#ifndef QTBOIDISASSEMBLYWINDOW_H
#define QTBOIDISASSEMBLYWINDOW_H

#include <QTextBrowser>
#include "../core/GameBoy.h"

class QtBoiDisassemblyWindow: public QTextBrowser
{
        Q_OBJECT

        public:
                QtBoiDisassemblyWindow(QWidget *parent, GameBoy *gb);
                ~QtBoiDisassemblyWindow();

                void gotoAddress(u16 addr);
                void gotoPC();

        private:
                GameBoy *gb;

};


#endif


