#ifndef QTBOIDISASSEMBLYWINDOW_H
#define QTBOIDISASSEMBLYWINDOW_H

#include <QTextBrowser>
#include <QString>
#include <QHash>
#include "../core/GameBoy.h"

class QtBoiDisassemblyWindow: public QTextBrowser
{
        Q_OBJECT

        public:
                QtBoiDisassemblyWindow(QWidget *parent, GameBoy *gb, QHash<u32, QString> *tags);
                ~QtBoiDisassemblyWindow();

                void gotoAddress(u16 addr);
                void gotoPC();
		void refresh();

        private:
		std::string insToHtml(const Instruction &ins);
		std::string operandToHtml(const Instruction::Operand &ins);
		std::string htmlLinkMem(u32 addr);
                GameBoy *gb;
		QString romTitle;
		QHash<u32, QString> *tags;

		u16 currentAddress;

};


#endif


