#include "QtBoiMainWindow.h"

#include <QMenu>
#include <QMenuBar>
#include <QString>
#include <QFileDialog>
#include <QPushButton>
#include <QColor>
#include <QPainter>
#include <iostream>

QtBoiMainWindow::QtBoiMainWindow(QWidget *parent)
  :QMainWindow(parent), emuThread(0)
{
  screen = new QImage(160, 144, QImage::Format_Indexed8);
  screen->setNumColors(8);
  screen->setColor(7, qRgb(0,0,0));
  screen->setColor(6, qRgb(36,36,36));
  screen->setColor(5, qRgb(73,73,73));
  screen->setColor(4, qRgb(109,109,109));
  screen->setColor(3, qRgb(146,146,146));
  screen->setColor(2, qRgb(182,182,182));
  screen->setColor(1, qRgb(219,219,219));
  screen->setColor(0, qRgb(255,255,255));

  createMenu();
  resize(640,480);
  centralWindow = new QLabel(this);
  setCentralWidget(centralWindow);


}

QtBoiMainWindow::~QtBoiMainWindow()
{
  if (emuThread) {
    emuThread->stop();
    delete emuThread;
  }
}

void QtBoiMainWindow::createMenu()
{
  loadROM       = new QAction("&Load ROM...", this);
  quit          = new QAction("&Quit", this);
  emulatorPause = new QAction("&Toggle pause", this);
  emulatorCont  = new QAction("&Go", this);
  emulatorStop  = new QAction("&Stop", this);
  emulatorStep  = new QAction("St&ep", this);

  QMenu *file;
  file = menuBar()->addMenu("&File");
  file->addAction(loadROM);
  file->addAction(quit);

  QMenu *emulator;
  emulator = menuBar()->addMenu("&Emulator");
  emulator->addAction(emulatorCont);
  emulator->addAction(emulatorPause);
  emulator->addAction(emulatorStop);

  QMenu *debug;
  debug = menuBar()->addMenu("&Debug");
  debug->addAction(emulatorStep);

  connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
  connect(loadROM, SIGNAL(triggered()), this, SLOT(onLoadROM()));
}

void QtBoiMainWindow::onLoadROM()
{
  QString filename = QFileDialog::getOpenFileName(this, tr("Load ROM"), "../roms", tr("GameBoy ROMs (*.gb *.gbc)"));
  if (filename == "") return;

  std::cout << filename.toStdString() << std::endl;
  
  if (emuThread) {
    emuThread->stop();
    emuThread->wait();
    delete emuThread;
  }

  emuThread = new QtBoiEmuThread(0, filename);
  
  connect(emulatorCont, SIGNAL(triggered()), emuThread, SLOT(cont()), Qt::DirectConnection);
  connect(emulatorStop, SIGNAL(triggered()), emuThread, SLOT(stop()), Qt::DirectConnection);
  connect(emulatorPause, SIGNAL(triggered()), emuThread, SLOT(toggle_paused()), Qt::DirectConnection);
  connect(emulatorStep, SIGNAL(triggered()), emuThread, SLOT(step()), Qt::DirectConnection);
  connect(emuThread, SIGNAL(redraw(const uchar*)), this, SLOT(onRedraw(const uchar*)));

  emuThread->start();
}

void QtBoiMainWindow::onRedraw(const uchar *buffer)
{
  uchar *pixels = screen->bits();
  memcpy(pixels, buffer, 160*144);
  //centralWindow->setPixmap(QPixmap::fromImage(screen->scaled(320,288)));
  centralWindow->setPixmap(QPixmap::fromImage(*screen));
}




