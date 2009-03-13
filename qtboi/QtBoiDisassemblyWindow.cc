#include <string>
#include <sstream>
#include <iomanip>

#include "QtBoiDisassemblyWindow.h"

QtBoiDisassemblyWindow::QtBoiDisassemblyWindow(QWidget *parent, GameBoy *gb)
        :gb(gb)
{
        setFocusPolicy(Qt::NoFocus);
        setMinimumSize(350,500);
}

QtBoiDisassemblyWindow::~QtBoiDisassemblyWindow()
{
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
        str << "<tr bgcolor=#c0ffc0><td width=25%>Addr</td><td width=25%>Opcodes</td><td>Instruction</td></tr>";

        bool hilightBG=true;

        while (pos < end)
        {
                Instruction ins(gb->disassemble_opcode(pos));
                str << "<tr bgcolor=" << (pos == gb->regs.PC ? "#ffc0c0" : (hilightBG ? "#d0d0d0" : "#ffffff")) << 
                        "><td>0x" << std::hex << std::setw(4) << std::setfill('0') << 
                        pos << "</td><td>";
                for (int i=0; i<ins.length; i++)
                        str << std::setw(2) << int(gb->memory.read(pos+i)) << " ";

                str << "</td><td>";

                str << ins.all << "</td></tr>";
                pos += ins.length;

                hilightBG = !hilightBG;
        }

        str << "</table></body></html>";

        setHtml(QString(str.str().c_str()));
}

void QtBoiDisassemblyWindow::gotoPC()
{
        gotoAddress(gb->regs.PC);
}

