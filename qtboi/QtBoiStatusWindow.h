#ifndef QTBOISTATUSWINDOW_H
#define QTBOISTATUSWINDOW_H

#include <QtWidgets/QTextBrowser>
#include "../core/GameBoy.h"

class QtBoiStatusWindow: public QTextBrowser
{
        Q_OBJECT

        public:
                QtBoiStatusWindow(QWidget *parent, GameBoy *gb);
                ~QtBoiStatusWindow();

                void update();

        private:
                GameBoy *gb;

};


#endif


