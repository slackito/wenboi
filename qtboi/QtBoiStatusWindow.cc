#include <iomanip>
#include <sstream>
#include <string>

#include "QtBoiStatusWindow.h"

QtBoiStatusWindow::QtBoiStatusWindow(QWidget *parent, GameBoy *gb) : gb(gb) {
  setFocusPolicy(Qt::NoFocus);
  setMinimumSize(200, 200);
  setMaximumWidth(320);
}

QtBoiStatusWindow::~QtBoiStatusWindow() {}

void QtBoiStatusWindow::update() {
  std::ostringstream str;

  str << "<html><head><title>Status</title></head><body>";

  str << "<table cellpadding=0 cellspacing=0><tr><td>";
  // 8-bit regs table
  str << "<table cellpadding=0 cellspacing=0>";
  str << "<tr bgcolor=#c0ffc0><th colspan=\"2\">8-bit</th></tr>";
  str << "<tr><td>A&nbsp;=&nbsp;</td><td>0x" << std::hex << std::setw(2)
      << std::setfill('0') << int(gb->regs.A) << "</td></tr>";
  str << "<tr><td>B&nbsp;=&nbsp;</td><td>0x" << std::hex << std::setw(2)
      << std::setfill('0') << int(gb->regs.B) << "</td></tr>";
  str << "<tr><td>C&nbsp;=&nbsp;</td><td>0x" << std::hex << std::setw(2)
      << std::setfill('0') << int(gb->regs.C) << "</td></tr>";
  str << "<tr><td>D&nbsp;=&nbsp;</td><td>0x" << std::hex << std::setw(2)
      << std::setfill('0') << int(gb->regs.D) << "</td></tr>";
  str << "<tr><td>E&nbsp;=&nbsp;</td><td>0x" << std::hex << std::setw(2)
      << std::setfill('0') << int(gb->regs.E) << "</td></tr>";
  str << "<tr><td>H&nbsp;=&nbsp;</td><td>0x" << std::hex << std::setw(2)
      << std::setfill('0') << int(gb->regs.H) << "</td></tr>";
  str << "<tr><td>L&nbsp;=&nbsp;</td><td>0x" << std::hex << std::setw(2)
      << std::setfill('0') << int(gb->regs.L) << "</td></tr>";
  str << "</table>";
  str << "</td><td width=20></td><td>";
  // 16-bit regs table
  str << "<table cellpadding=0 cellspacing=0>";
  str << "<tr bgcolor=#c0ffc0><th colspan=\"2\">16-bit</th></tr>";
  str << "<tr><td>BC&nbsp;=&nbsp;</td><td>0x" << std::hex << std::setw(4)
      << std::setfill('0') << int(gb->regs.BC) << "</td></tr>";
  str << "<tr><td>DE&nbsp;=&nbsp;</td><td>0x" << std::hex << std::setw(4)
      << std::setfill('0') << int(gb->regs.DE) << "</td></tr>";
  str << "<tr><td>HL&nbsp;=&nbsp;</td><td>0x" << std::hex << std::setw(4)
      << std::setfill('0') << int(gb->regs.HL) << "</td></tr>";
  str << "<tr></tr>";
  str << "<tr><td>SP&nbsp;=&nbsp;</td><td>0x" << std::hex << std::setw(4)
      << std::setfill('0') << int(gb->regs.SP) << "</td></tr>";
  str << "<tr></tr>";
  str << "<tr><td>PC&nbsp;=&nbsp;</td><td>0x" << std::hex << std::setw(4)
      << std::setfill('0') << int(gb->regs.PC) << "</td></tr>";
  str << "</table>";
  str << "</td><td width=20></td><td>";
  // Flags & state table (TODO: Interpret IE and IF registers)
  str << "<table cellpadding=0 cellspacing=0>";
  str << "<tr bgcolor=#c0ffc0><th colspan=\"2\">Flags</th></tr>";
  str << "<tr><td bgcolor="
      << (gb->check_flag(GameBoy::ZERO_FLAG) ? "#FF0000" : "#FFFFFF")
      << ">Z</td></tr>";
  str << "<tr><td bgcolor="
      << (gb->check_flag(GameBoy::ADD_SUB_FLAG) ? "#FF0000" : "#FFFFFF")
      << ">N</td></tr>";
  str << "<tr><td bgcolor="
      << (gb->check_flag(GameBoy::HALF_CARRY_FLAG) ? "#FF0000" : "#FFFFFF")
      << ">H</td></tr>";
  str << "<tr><td bgcolor="
      << (gb->check_flag(GameBoy::CARRY_FLAG) ? "#FF0000" : "#FFFFFF")
      << ">C</td></tr>";
  str << "<tr><td>IME&nbsp;=&nbsp;" << int(gb->IME) << "</td></tr>";
  int IE = int(gb->memory.read(0xFFFF, GBMemory::DONT_WATCH));
  str << "<tr><td>IE&nbsp;&nbsp;=";
  if (IE & GameBoy::IRQ_VBLANK)
    str << "&nbsp;VBL";
  if (IE & GameBoy::IRQ_LCD_STAT)
    str << "&nbsp;LCD";
  if (IE & GameBoy::IRQ_TIMER)
    str << "&nbsp;TIM";
  if (IE & GameBoy::IRQ_SERIAL)
    str << "&nbsp;SER";
  if (IE & GameBoy::IRQ_JOYPAD)
    str << "&nbsp;JOYP";
  str << "</td></tr>";
  int IF = int(gb->memory.read(0xFF0F, GBMemory::DONT_WATCH));
  str << "<tr><td>IF&nbsp;&nbsp;=";
  if (IF & GameBoy::IRQ_VBLANK)
    str << "&nbsp;VBL";
  if (IF & GameBoy::IRQ_LCD_STAT)
    str << "&nbsp;LCD";
  if (IF & GameBoy::IRQ_TIMER)
    str << "&nbsp;TIM";
  if (IF & GameBoy::IRQ_SERIAL)
    str << "&nbsp;SER";
  if (IF & GameBoy::IRQ_JOYPAD)
    str << "&nbsp;JOYP";
  str << "</td></tr>";
  str << "</table>";
  str << "</td></tr>";
  str << "</table><hr>";

  str << "<table>";
  str << "<tr bgcolor=#c0ffc0><th colspan=\"2\">Breakpoints</th></tr>";
  GameBoy::BreakpointMap bpmap = gb->get_breakpoints();
  for (GameBoy::BreakpointMap::iterator i = bpmap.begin(); i != bpmap.end();
       i++) {
    str << "<tr><td>&nbsp;" << i->first << "&nbsp;</td>";
    str << "<td>0x" << std::hex << i->second.addr
        << (i->second.enabled ? "" : "(disabled)") << "</td></tr>";
  }

  str << "</table></body></html>";

  setHtml(QString(str.str().c_str()));
}
