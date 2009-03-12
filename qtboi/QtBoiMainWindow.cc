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
	screen->setNumColors(7);
	screen->setColor(6, qRgb(0,0,0));
	screen->setColor(5, qRgb(42,42,42));
	screen->setColor(4, qRgb(85,85,85));
	screen->setColor(3, qRgb(127,127,127));
	screen->setColor(2, qRgb(170,170,170));
	screen->setColor(1, qRgb(212,212,212));
	screen->setColor(0, qRgb(255,255,255));

	createMenu();
	resize(640,480);
	centralWindow = new QLabel(this);
	setCentralWidget(centralWindow);

	emuThread = new QtBoiEmuThread(this);
	emuThread->start();

	connect(emulatorCont, SIGNAL(triggered()), emuThread, SLOT(cont()));
	connect(emulatorStop, SIGNAL(triggered()), emuThread, SLOT(stop()));
	connect(emulatorPause, SIGNAL(triggered()), emuThread, SLOT(pause()));
	connect(emulatorStep, SIGNAL(triggered()), emuThread, SLOT(step()));
	connect(emulatorReset, SIGNAL(triggered()), emuThread, SLOT(reset()));
	connect(emuThread, SIGNAL(redraw(const uchar*)), this, SLOT(onRedraw(const uchar*)));

}

QtBoiMainWindow::~QtBoiMainWindow()
{
	if (emuThread) {
		emuThread->stop();
		emuThread->wait();
		delete emuThread;
	}
}

void QtBoiMainWindow::createMenu()
{
	loadROM       = new QAction("&Load ROM...", this);
	quit          = new QAction("&Quit", this);
	emulatorPause = new QAction("&Pause", this);
	emulatorCont  = new QAction("&Go", this);
	emulatorStop  = new QAction("&Stop", this);
	emulatorStep  = new QAction("St&ep", this);
	emulatorReset = new QAction("&Reset", this);


	QMenu *file;
	file = menuBar()->addMenu("&File");
	file->addAction(loadROM);
	file->addAction(quit);

	QMenu *emulator;
	emulator = menuBar()->addMenu("&Emulator");
	emulator->addAction(emulatorCont);
	emulator->addAction(emulatorPause);
	emulator->addAction(emulatorStop);
	emulator->addAction(emulatorReset);

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

	emuThread->loadROM(filename);

}

void QtBoiMainWindow::onRedraw(const uchar *buffer)
{
	uchar *pixels = screen->bits();
	memcpy(pixels, buffer, 160*144);
	centralWindow->setPixmap(QPixmap::fromImage(screen->scaled(320,288)));
}




