#include <string>
#include <sstream>
#include <iomanip>

#include "QtBoiDisassemblyWindow.h"
#include "../common/toString.h"

QtBoiDisassemblyWindow::QtBoiDisassemblyWindow(QWidget *parent, GameBoy *gb, QHash<u32,QString> *tags)
        :gb(gb), tags(tags)
{
        setFocusPolicy(Qt::NoFocus);
        setMinimumSize(500,500);
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

	std::string result(ins.str);
	result += " ";
	if (ins.str.substr(0,2)=="JR")
	{
		result += operandToHtml(ins.op1);
		result += " [";
		result += htmlLinkMem(ins.op2.val);
		result += "]";
	}
	else if (ins.str.substr(0,2)=="JP" || ins.str.substr(0,4)=="CALL")
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

        setHtml(QString(str.str().c_str()));
	currentAddress=addr;
}

void QtBoiDisassemblyWindow::gotoPC()
{
        gotoAddress(gb->regs.PC);
}

void QtBoiDisassemblyWindow::refresh()
{
        gotoAddress(currentAddress);
}



