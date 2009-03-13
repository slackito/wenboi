#include "QtBoiMainWindow.h"

#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
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

	emuThread = new QtBoiEmuThread(this);
	emuThread->start();

	loadROM       = new QAction(tr("&Load ROM..."), this);
	quit          = new QAction(tr("&Quit"), this);
	emulatorPause = new QAction(tr("&Pause"), this);
	emulatorCont  = new QAction(tr("&Go"), this);
	emulatorStop  = new QAction(tr("&Stop"), this);
	emulatorStep  = new QAction(tr("St&ep"), this);
	emulatorReset = new QAction(tr("&Reset"), this);

	loadROM->setShortcut(QKeySequence(tr("Ctrl+O", "File|Load ROM...")));
	emulatorCont->setShortcut(QKeySequence(tr("F5", "Emulator|Go")));
	emulatorStep->setShortcut(QKeySequence(tr("F7", "Debug|Step")));
	//emulatorCont->setIcon(QIcon("../icons/player_play.svg"));
	//emulatorPause->setIcon(QIcon("../icons/player_pause.svg"));
	//loadROM->setIcon(QIcon("../icons/player_eject.svg"));

	createMenu();
	createToolbar();


	connect(emulatorCont, SIGNAL(triggered()), emuThread, SLOT(cont()));
	connect(emulatorStop, SIGNAL(triggered()), emuThread, SLOT(stop()));
	connect(emulatorPause, SIGNAL(triggered()), emuThread, SLOT(pause()));
	connect(emulatorStep, SIGNAL(triggered()), emuThread, SLOT(step()));
	connect(emulatorReset, SIGNAL(triggered()), emuThread, SLOT(reset()));
	connect(emuThread, SIGNAL(redraw(const uchar*)), this, SLOT(onRedraw(const uchar*)));

	resize(640,480);
	centralWindow = new QLabel(this);
	setCentralWidget(centralWindow);
	centralWindow->move(0,0);
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
	QMenu *file;
	file = menuBar()->addMenu(tr("&File"));
	file->addAction(loadROM);
	file->addAction(quit);

	QMenu *emulator;
	emulator = menuBar()->addMenu(tr("&Emulator"));
	emulator->addAction(emulatorCont);
	emulator->addAction(emulatorPause);
	emulator->addAction(emulatorStop);
	emulator->addAction(emulatorReset);

	QMenu *debug;
	debug = menuBar()->addMenu(tr("&Debug"));
	debug->addAction(emulatorStep);

	connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(loadROM, SIGNAL(triggered()), this, SLOT(onLoadROM()));
}

void QtBoiMainWindow::createToolbar()
{
	QToolBar *toolbar;
	toolbar = addToolBar(tr("&Emulator"));
	toolbar->addAction(loadROM);
	toolbar->addAction(emulatorCont);
	toolbar->addAction(emulatorPause);
	toolbar->addAction(emulatorReset);
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

void QtBoiMainWindow::keyPressEvent(QKeyEvent *event)
{
	if (emuThread->isPaused) {
		QMainWindow::keyPressEvent(event);
		return;
	}
	switch(event->key())
	{
		case Qt::Key_Up:
		emuThread->pressControl(GameBoy::PAD_UP);
		break;
		case Qt::Key_Down:
		emuThread->pressControl(GameBoy::PAD_DOWN);
		break;
		case Qt::Key_Left:
		emuThread->pressControl(GameBoy::PAD_LEFT);
		break;
		case Qt::Key_Right:
		emuThread->pressControl(GameBoy::PAD_RIGHT);
		break;
		case Qt::Key_Z:
		emuThread->pressControl(GameBoy::BUTTON_A);
		break;
		case Qt::Key_X:
		emuThread->pressControl(GameBoy::BUTTON_B);
		break;
		case Qt::Key_Return:
		emuThread->pressControl(GameBoy::BUTTON_SELECT);
		break;
		case Qt::Key_Space:
		emuThread->pressControl(GameBoy::BUTTON_START);
		break;
		default:
		QMainWindow::keyPressEvent(event);
	}
}

void QtBoiMainWindow::keyReleaseEvent(QKeyEvent *event)
{
	if (emuThread->isPaused) {
		QMainWindow::keyReleaseEvent(event);
		return;
	}
	switch(event->key())
	{
		case Qt::Key_Up:
		emuThread->releaseControl(GameBoy::PAD_UP);
		break;
		case Qt::Key_Down:
		emuThread->releaseControl(GameBoy::PAD_DOWN);
		break;
		case Qt::Key_Left:
		emuThread->releaseControl(GameBoy::PAD_LEFT);
		break;
		case Qt::Key_Right:
		emuThread->releaseControl(GameBoy::PAD_RIGHT);
		break;
		case Qt::Key_Z:
		emuThread->releaseControl(GameBoy::BUTTON_A);
		break;
		case Qt::Key_X:
		emuThread->releaseControl(GameBoy::BUTTON_B);
		break;
		case Qt::Key_Return:
		emuThread->releaseControl(GameBoy::BUTTON_SELECT);
		break;
		case Qt::Key_Space:
		emuThread->releaseControl(GameBoy::BUTTON_START);
		break;
		default:
		QMainWindow::keyReleaseEvent(event);
	}
}




