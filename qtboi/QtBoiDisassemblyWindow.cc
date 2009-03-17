#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>

#include "QtBoiDisassemblyWindow.h"
#include "../common/toString.h"

QtBoiDisassemblyWindow::QtBoiDisassemblyWindow(QWidget *parent, GameBoy *gb, QHash<u32,QString> *tags)
        :QWidget(parent), gb(gb), tags(tags), history(), historyPosition(-1)
{
	setFocusPolicy(Qt::NoFocus);

	browser = new QTextBrowser(this);
        browser->setOpenLinks(false);
	browser->setFont(QFont("courier"));
        browser->setFocusPolicy(Qt::NoFocus);
        browser->setMinimumSize(500,500);
	connect(browser, SIGNAL(anchorClicked(const QUrl&)), this, SIGNAL(anchorClicked(const QUrl &)));


	backButton    = new QPushButton("Back", this);
	backButton->setEnabled(false);
        backButton->setFocusPolicy(Qt::NoFocus);
	forwardButton = new QPushButton("Forward", this);
	forwardButton->setEnabled(false);
        forwardButton->setFocusPolicy(Qt::NoFocus);
	connect(backButton, SIGNAL(clicked()), this, SLOT(historyBack()));
	connect(forwardButton, SIGNAL(clicked()), this, SLOT(historyForward()));
	
	//backButton->setIcon(QIcon("../icons/go-next.svg"));
	//forwardButton->setIcon(QIcon("../icons/go-previous.svg")); 
	
	QHBoxLayout *buttons = new QHBoxLayout();
	buttons->addWidget(backButton);
	buttons->addWidget(forwardButton);

	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(browser);
	vbox->addLayout(buttons);

	setLayout(vbox);
}

QtBoiDisassemblyWindow::~QtBoiDisassemblyWindow()
{
}

std::string QtBoiDisassemblyWindow::htmlLinkMem(u32 addr)
{
	std::string result("<a href=\"gotoaddr:");
	result += toString(addr);
	result += "\">";
	if (tags->value(addr) != "")
		result += tags->value(addr).toStdString();
	else
		result += toStringHex(addr, 4);
	result += "</a>";

	return result;
}

std::string QtBoiDisassemblyWindow::operandToHtml(const Instruction::Operand &op)
{
	std::string result;
	switch(op.type)  
	{
		case Instruction::MEM_DIRECT:
		result += "(";
		result += htmlLinkMem(op.val);
		result += ")";
		break;

		case Instruction::INM16:
		result += toStringHex(op.val,4);
		break;

		default:
		return op.str;
	}

	return result;
}


std::string QtBoiDisassemblyWindow::insToHtml(const Instruction &ins)
{

	std::string result;

	if (ins.type == Instruction::RESET)
	{
		result += "RST ";
		result += htmlLinkMem(strtol(ins.str.substr(4).c_str(), 0, 16));
	}
	else
	{
		result += ins.str;
		result += " ";
		if (ins.str.substr(0,2)=="JR")
		{
			result += operandToHtml(ins.op1);
			result += " [";
			result += htmlLinkMem(ins.op2.val);
			result += "]";
		}
		else if ((ins.str.substr(0,2)=="JP" && ins.op1.type == Instruction::INM16)
				|| ins.str.substr(0,4)=="CALL")
		{
			result += htmlLinkMem(ins.op1.val);
		}
		else
		{
			result += operandToHtml(ins.op1);
			if (ins.op2.type != Instruction::NONE)
			{
				result += ", ";
				result += operandToHtml(ins.op2);
			}
		}
	}
	return result;
}

void QtBoiDisassemblyWindow::gotoAddress(u16 addr)
{
        int start = addr;
        int end   = start+200;
        int pos   = start;

        if (end > 0xFFFF) end = 0xFFFF;
        std::ostringstream str;
        
        str << "<html><head><title>Disassembly</title></head><body>";
        str << "<table cellpadding=0 cellspacing=0>";
        str << "<tr bgcolor=#c0ffc0><td width=25%>Labels</td><td width=15%>Addr</td><td width=15%>Opcodes</td><td>Instruction</td></tr>";

        bool hilightBG=true;

        while (pos < end)
        {
                Instruction ins(gb->disassemble_opcode(pos));
                str << "<tr bgcolor=" << (pos == gb->regs.PC ? "#ffc0c0" : (hilightBG ? "#d0d0d0" : "#ffffff")) << ">" <<
			"<td>" << tags->value(pos).toStdString() << "</td>" <<
			"<td><a href=\"newtag:" << std::dec << pos << "\">0x" << std::hex << std::setw(4) << std::setfill('0') << 
                        pos << "</a></td><td>";
                for (int i=0; i<ins.length; i++)
                        str << std::setw(2) << int(gb->memory.read(pos+i)) << " ";

                str << "</td><td>";

                str << insToHtml(ins) << "</td></tr>";
                pos += ins.length;

                hilightBG = !hilightBG;
        }

        str << "</table></body></html>";

        browser->setHtml(QString(str.str().c_str()));
	currentAddress=addr;

	// If there are "forward" history elements
	// - check if we are going forwards or backwards
	// - else, delete forward history and insert the new address
	if (historyPosition < history.size()-1 && history[historyPosition+1] == addr)
		historyPosition++;
	else if (historyPosition > 0 && history[historyPosition-1] == addr)
		historyPosition--;
	else
	{
		while (history.size() > historyPosition+1)
			history.pop_back();
		history.push_back(addr);
		historyPosition++;
	}

	// enable/disable navigation buttons
	if (historyPosition >= 1) backButton->setEnabled(true);
	else backButton->setEnabled(false);

	if (historyPosition >= 0 && historyPosition < history.size()-1) forwardButton->setEnabled(true);
	else forwardButton->setEnabled(false);
}

void QtBoiDisassemblyWindow::gotoPC()
{
        gotoAddress(gb->regs.PC);
}

void QtBoiDisassemblyWindow::refresh()
{
        gotoAddress(currentAddress);
}

void QtBoiDisassemblyWindow::historyBack()
{
	if (historyPosition >= 1)
		gotoAddress(history[historyPosition-1]);
}

void QtBoiDisassemblyWindow::historyForward()
{
	if (historyPosition < history.size()-1)
		gotoAddress(history[historyPosition+1]);
}



