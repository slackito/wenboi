#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QString>
#include <QFileDialog>
#include <QInputDialog>
#include <QPushButton>
#include <QColor>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>

#include <iostream>

#include "QtBoiMainWindow.h"
#include "../core/GameBoy.h"
#include "../core/GBRom.h"

QtBoiMainWindow::QtBoiMainWindow(QWidget *parent)
	:QMainWindow(parent), emuThread(0)
{
	screenImage = new QImage(160, 144, QImage::Format_Indexed8);
	screenImage->setNumColors(7);
	// gray palette
	//screenImage->setColor(6, qRgb(0,0,0));
	//screenImage->setColor(5, qRgb(42,42,42));
	//screenImage->setColor(4, qRgb(85,85,85));
	//screenImage->setColor(3, qRgb(127,127,127));
	//screenImage->setColor(2, qRgb(170,170,170));
	//screenImage->setColor(1, qRgb(212,212,212));
	//screenImage->setColor(0, qRgb(255,255,255));
	
	// greenish palette
	screenImage->setColor(6, qRgb(64,64,64));
	screenImage->setColor(5, qRgb(82,82,53));
	screenImage->setColor(4, qRgb(101,101,42));
	screenImage->setColor(3, qRgb(120,120,31));
	screenImage->setColor(2, qRgb(139,139,21));
	screenImage->setColor(1, qRgb(166,166,10));
	screenImage->setColor(0, qRgb(192,192,0));

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
	emulatorPause->setShortcut(QKeySequence(tr("F6", "Emulator|Pause")));
	emulatorStep->setShortcut(QKeySequence(tr("F7", "Debug|Step")));
	//emulatorCont->setIcon(QIcon("../icons/player_play.svg"));
	//emulatorPause->setIcon(QIcon("../icons/player_pause.svg"));
	//loadROM->setIcon(QIcon("../icons/player_eject.svg"));

	createMenu();
	createToolbar();

        statusbar = statusBar();


	connect(emulatorCont, SIGNAL(triggered()), emuThread, SLOT(cont()));
	connect(emulatorCont, SIGNAL(triggered()), this, SLOT(onResume()));
	connect(emulatorStop, SIGNAL(triggered()), emuThread, SLOT(stop()));
	connect(emulatorPause, SIGNAL(triggered()), emuThread, SLOT(pause()));
	connect(emulatorStep, SIGNAL(triggered()), emuThread, SLOT(step()));
	connect(emulatorReset, SIGNAL(triggered()), emuThread, SLOT(reset()));
	connect(emuThread, SIGNAL(redraw(const uchar*)), this, SLOT(onRedraw(const uchar*)));
	connect(emuThread, SIGNAL(emulationPaused()), this, SLOT(onPause()));

	resize(800,600);
	centralWindow = new QWidget(this);
	setCentralWidget(centralWindow);

        QHBoxLayout *topHBoxLayout = new QHBoxLayout;

        QWidget *leftVBox  = new QWidget(centralWindow);
        QWidget *rightVBox = new QWidget(centralWindow);
	QVBoxLayout *leftVBoxLayout = new QVBoxLayout;
	QVBoxLayout *rightVBoxLayout = new QVBoxLayout;
        leftVBox->setLayout(leftVBoxLayout);
        rightVBox->setLayout(rightVBoxLayout);

	screen = new QLabel(centralWindow);
	screen->resize(320,288);
        uchar buf[160*144];
        memset(buf, 0, 160*144);
        onRedraw(buf);

	status = new QtBoiStatusWindow(centralWindow, &emuThread->gb);
	status->setFont(QFont("courier"));

        topHBoxLayout->addWidget(leftVBox);
        topHBoxLayout->addWidget(rightVBox);
	leftVBoxLayout->addWidget(screen);
	leftVBoxLayout->addWidget(status);
        
        disassembly = new QtBoiDisassemblyWindow(centralWindow, &emuThread->gb, &tags);
        disassembly->setOpenLinks(false);
	disassembly->setFont(QFont("courier"));

	connect(disassembly, SIGNAL(anchorClicked(const QUrl&)), this, SLOT(onDisassemblyAnchorClicked(const QUrl&)));
        
        rightVBoxLayout->addWidget(disassembly);
	
	centralWindow->setLayout(topHBoxLayout);

}

QtBoiMainWindow::~QtBoiMainWindow()
{
	if (romTitle != "")
		saveTags();
	
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
	saveTags();

	QString filename = QFileDialog::getOpenFileName(this, tr("Load ROM"), "../roms", tr("GameBoy ROMs (*.gb *.gbc)"));
	if (filename == "") return;

	emuThread->loadROM(filename);
	
	char title[12];
	memcpy(title, emuThread->gb.rom->header.new_title, 11);
	title[11]='\0';
	romTitle=QString(title);
	loadTags();

        statusbar->showMessage(tr("Loaded ROM ")+filename+" ["+romTitle+"]");
}

void QtBoiMainWindow::onDisassemblyAnchorClicked(const QUrl& url)
{
	std::cout << url.toString().toStdString() << std::endl;
	if (url.scheme() == "gotoaddr") {
		u32 addr = url.path().toUInt();
		disassembly->gotoAddress(addr);
	} 
	else if (url.scheme() == "newtag") {
		u32 addr = url.path().toUInt();
		QString tag = QInputDialog::getText(this, tr("Create new tag"), tr("Enter the tag for the selected address"),
				QLineEdit::Normal, tags[addr]);

		tags[addr] = tag;
		disassembly->refresh();
	}
}

void QtBoiMainWindow::onRedraw(const uchar *buffer)
{
	uchar *pixels = screenImage->bits();
	memcpy(pixels, buffer, 160*144);
	screen->setPixmap(QPixmap::fromImage(screenImage->scaled(320,288)));
}

void QtBoiMainWindow::onPause()
{
	status->setText(QString(emuThread->gb.status_string().c_str()));
        disassembly->gotoPC();
        status->update();
        statusbar->showMessage(tr("Emulation paused", "Status bar emulation paused msg"));
}

void QtBoiMainWindow::onResume()
{
        statusbar->showMessage(tr("Emulation running", "Status bar emulation running msg"));
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




void QtBoiMainWindow::loadTags()
{
	this->romTitle = romTitle;
	tags.clear();
	QFile tagsfile(romTitle+".tags");
	if (!tagsfile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream in(&tagsfile);
	while(!in.atEnd())
	{
		u32 addr;
		QString tag;

		in >> addr;
		in.skipWhiteSpace();
		tag = in.readLine();

		tags[addr] = tag;
	}
}

void QtBoiMainWindow::saveTags()
{
	QFile tagsfile(romTitle+".tags");
	if (!tagsfile.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&tagsfile);
	QHashIterator<u32,QString> i(tags);
	while (i.hasNext())
	{
		i.next();
		out << i.key() << " " << i.value() << "\n";
	}
}


