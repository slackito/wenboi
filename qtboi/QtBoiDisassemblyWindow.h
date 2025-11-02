#ifndef QTBOIDISASSEMBLYWINDOW_H
#define QTBOIDISASSEMBLYWINDOW_H

#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QPushButton>
#include <QString>
#include <QHash>
#include <QList>
#include <QUrl>
#include "../core/GameBoy.h"

class QtBoiDisassemblyWindow: public QWidget
{
    Q_OBJECT

    public:
        QtBoiDisassemblyWindow(QWidget *parent, GameBoy *gb, QHash<u32, QString> *tags);
        ~QtBoiDisassemblyWindow();

        void gotoAddress(u16 addr);
        void gotoPC();
		void refresh();
        void ready();

    public slots:
        void historyBack();
		void historyForward();
        void onGotoButton();

    signals:
        void anchorClicked(const QUrl & link);

    private:
        std::string insToHtml(const Instruction &ins);
        std::string operandToHtml(const Instruction::Operand &ins);
        std::string htmlLinkMem(u32 addr);

        QTextBrowser *browser;
        QPushButton *backButton, *forwardButton, *gotoButton;

        GameBoy *gb;
        QString romTitle;
        QHash<u32, QString> *tags;
        QList<u32> history;
        int historyPosition;
        u16 currentAddress;
};


#endif


