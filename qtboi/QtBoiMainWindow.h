#ifndef QTBOIMAINWINDOW_H
#define QTBOIMAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QImage>
#include <QLabel>
#include "QtBoiEmuThread.h"

class QtBoiMainWindow: public QMainWindow
{
  Q_OBJECT

  public:
    QtBoiMainWindow(QWidget *parent=0);
    ~QtBoiMainWindow();

  public slots:
    void onLoadROM();
    void onRedraw(const uchar *buffer);

  private:
    void createMenu();
    QtBoiEmuThread *emuThread;

    QLabel *centralWindow;
    QImage *screen;

    QAction *loadROM;
    QAction *quit;
    QAction *emulatorPause;
    QAction *emulatorCont;
    QAction *emulatorStop;
    QAction *emulatorStep;
};


#endif


