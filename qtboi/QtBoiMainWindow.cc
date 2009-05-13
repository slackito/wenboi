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
	screenImage = new QImage(160,144, QImage::Format_RGB32);
	scaledImage = new QImage(320,288, QImage::Format_RGB32);
	scalingMethod = SCALING_QIMAGE;

	// greenish palette
	/*
	screenImage->setColor(6, qRgb(64,64,64));
	screenImage->setColor(5, qRgb(82,82,53));
	screenImage->setColor(4, qRgb(101,101,42));
	screenImage->setColor(3, qRgb(120,120,31));
	screenImage->setColor(2, qRgb(139,139,21));
	screenImage->setColor(1, qRgb(166,166,10));
	screenImage->setColor(0, qRgb(192,192,0));
	*/

	emuThread = new QtBoiEmuThread(this);
	emuThread->start();
	connect(emuThread, SIGNAL(redraw(const uchar*)), this, SLOT(onRedraw(const uchar*)));
	connect(emuThread, SIGNAL(emulationPaused()), this, SLOT(onPause()));

	createActions();
	createMenu();
	createToolbar();
    statusbar = statusBar();

	//resize(800,600);
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
	status = new QtBoiStatusWindow(centralWindow, &emuThread->gb);
	status->setFont(QFont("courier"));

    topHBoxLayout->addWidget(leftVBox);
    topHBoxLayout->addWidget(rightVBox);
	leftVBoxLayout->addWidget(screen);
	leftVBoxLayout->addWidget(status);
        
    disassembly = new QtBoiDisassemblyWindow(centralWindow, &emuThread->gb, &tags);
	connect(disassembly, SIGNAL(anchorClicked(const QUrl&)), this, SLOT(onDisassemblyAnchorClicked(const QUrl&)));
    rightVBoxLayout->addWidget(disassembly);

	centralWindow->setLayout(topHBoxLayout);

	// draw blank screen, set default visibility for subwindows
	onViewDisassemblyWindow();
	onViewStatusWindow();
    uchar buf[160*144];
    memset(buf, 0, 160*144);
    onRedraw(buf);
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


void QtBoiMainWindow::createActions()
{
	loadROM       = new QAction(tr("&Load ROM..."), this);
	quit          = new QAction(tr("&Quit"), this);
	emulatorPause = new QAction(tr("&Pause"), this);
	emulatorCont  = new QAction(tr("&Go"), this);
	emulatorStop  = new QAction(tr("&Stop"), this);
	emulatorStep  = new QAction(tr("St&ep"), this);
	emulatorReset = new QAction(tr("&Reset"), this);
	
	viewDisassemblyWindow = new QAction(tr("&Disassembly window"), this);
	viewStatusWindow      = new QAction(tr("&Status window"), this);
	viewDisassemblyWindow->setCheckable(true);
	viewStatusWindow->setCheckable(true);
	viewDisassemblyWindow->setChecked(true);
	viewStatusWindow->setChecked(true);

	scalingGroup   = new QActionGroup(this);
	scalingNone    = new QAction(tr("&None"), scalingGroup);
	scalingQImage  = new QAction(tr("&QImage"), scalingGroup);
	scalingScale2X = new QAction(tr("Scale&2X"), scalingGroup);
	scalingNone->setCheckable(true);
	scalingQImage->setCheckable(true);
	scalingScale2X->setCheckable(true);
	scalingQImage->setChecked(true);


	loadROM->setShortcut(QKeySequence(tr("Ctrl+O", "File|Load ROM...")));
	emulatorCont->setShortcut(QKeySequence(tr("F5", "Emulator|Go")));
	emulatorPause->setShortcut(QKeySequence(tr("F6", "Emulator|Pause")));
	emulatorStep->setShortcut(QKeySequence(tr("F7", "Debug|Step")));
	//emulatorCont->setIcon(QIcon("../icons/player_play.svg"));
	//emulatorPause->setIcon(QIcon("../icons/player_pause.svg"));
	//loadROM->setIcon(QIcon("../icons/fileopen.svg"));

	connect(emulatorCont, SIGNAL(triggered()), emuThread, SLOT(cont()));
	connect(emulatorCont, SIGNAL(triggered()), this, SLOT(onResume()));
	connect(emulatorStop, SIGNAL(triggered()), emuThread, SLOT(stop()));
	connect(emulatorPause, SIGNAL(triggered()), emuThread, SLOT(pause()));
	connect(emulatorStep, SIGNAL(triggered()), emuThread, SLOT(step()));
	connect(emulatorReset, SIGNAL(triggered()), emuThread, SLOT(reset()));
	connect(viewDisassemblyWindow, SIGNAL(triggered()), this, SLOT(onViewDisassemblyWindow()));
	connect(viewStatusWindow, SIGNAL(triggered()), this, SLOT(onViewStatusWindow()));
	connect(scalingNone, SIGNAL(triggered()), this, SLOT(onScalingNone()));
	connect(scalingQImage, SIGNAL(triggered()), this, SLOT(onScalingQImage()));
	connect(scalingScale2X, SIGNAL(triggered()), this, SLOT(onScalingScale2X()));
}


void QtBoiMainWindow::createMenu()
{
	QMenu *file;
	file = menuBar()->addMenu(tr("&File"));
	file->addAction(loadROM);
	file->addAction(quit);

	QMenu *view;
	view = menuBar()->addMenu(tr("&View"));
	view->addAction(viewDisassemblyWindow);
	view->addAction(viewStatusWindow);

	QMenu *viewScalingMethod;
	viewScalingMethod = view->addMenu(tr("&Scaling method"));
	viewScalingMethod->addAction(scalingNone);
	viewScalingMethod->addAction(scalingQImage);
	viewScalingMethod->addAction(scalingScale2X);

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
    toolbar->addAction(emulatorStep);
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
    disassembly->ready();
}

void QtBoiMainWindow::onDisassemblyAnchorClicked(const QUrl& url)
{
	//std::cout << url.toString().toStdString() << std::endl;
	if (url.scheme() == "gotoaddr") {
		u32 addr = url.path().toUInt();
		disassembly->gotoAddress(addr);
	} 
	else if (url.scheme() == "tag") {
		u32 addr = url.path().toUInt();
        QStringList tag_comment = tags.value(addr,"#").split("#");
        QString tag     = tag_comment.at(0);
        QString comment = tag_comment.at(1);
		tag = QInputDialog::getText(this, tr("Edit tag"), tr("Enter the tag for the selected address"),
				QLineEdit::Normal, tag);
        tags[addr] = tag+"#"+comment;
		disassembly->refresh();
	}
    else if (url.scheme() == "togglebp") {
        int addr = url.path().toUInt();
        bool bpFound = false;
        GameBoy::BreakpointMap bpmap = emuThread->gb.get_breakpoints();
        for (GameBoy::BreakpointMap::iterator i=bpmap.begin(); i != bpmap.end(); i++) {
            if (i->second.addr == addr) {
                emuThread->gb.delete_breakpoint(i->first);
                bpFound = true;
                break;
            }
        }
        if (!bpFound) {
            emuThread->gb.set_breakpoint(addr);
            //std::cout << "bp set at 0x" << std::hex << addr << std::endl;
        }
        disassembly->refresh();
        status->update();
    }
    else if (url.scheme() == "comment") {
		u32 addr = url.path().toUInt();
        QStringList tag_comment = tags.value(addr,"#").split("#");
        QString tag     = tag_comment.at(0);
        QString comment = tag_comment.at(1);
		comment = QInputDialog::getText(this, tr("Edit comment"), tr("Enter the comment for the selected address"),
				QLineEdit::Normal, comment);
        tags[addr] = tag+"#"+comment;
		disassembly->refresh();
    }
}

void QtBoiMainWindow::onRedraw(const uchar *buffer)
{
	
	uint *pixels = reinterpret_cast<uint*>(screenImage->bits());
	//memcpy(pixels, buffer, 160*144);
	for (int y=0; y<144; y++)
		for (int x=0; x<160; x++) {
			unsigned int val = 255-buffer[160*y+x]*42;
			pixels[160*y+x]=(val<<16)|(val<<8)|val;
		}

	switch(scalingMethod){
		case SCALING_QIMAGE:
			screen->setPixmap(QPixmap::fromImage(screenImage->scaled(320,288)));
			break;
		case SCALING_SCALE2X:
			scale2x(screenImage, scaledImage);
			screen->setPixmap(QPixmap::fromImage(*scaledImage));
			break;
		case SCALING_NONE:
		default:
			screen->setPixmap(QPixmap::fromImage(*screenImage));
	}
}


// dst size must be 2*src size
void QtBoiMainWindow::scale2x(const QImage *src, QImage *dst)
{
	uint *dst_pixels = reinterpret_cast<uint*>(dst->bits());
	const uint *src_pixels = reinterpret_cast<const uint*>(src->bits());
	int src_w = src->width();
	int src_h = src->height();
	// scale2x
	for (int y=0; y < src_h; y++) {
		for (int x=0; x < src_w; x++) {
			uint A,B,C,D,E,F,G,H,I,E0,E1,E2,E3;
			E=src_pixels[src_w*y+x];
			if (x > 0) {
				D=src_pixels[src_w*y+(x-1)];
				if (y > 0) {
					A = src_pixels[src_w*(y-1)+(x-1)];
					B = src_pixels[src_w*(y-1)+x];
				} else {
					A = D;
					B = D;
				}
				if (y < src_h-1) {
					G = src_pixels[src_w*(y+1)+(x-1)];
					H = src_pixels[src_w*(y+1)+x];
				} else {
					G = D;
					H = D;
				}
			} else { // x==0
				D=E;
				if (y > 0) {
					A = src_pixels[src_w*(y-1)];
					B = A;
				} else {
					A = D;
					B = D;
				}
				if (y < src_h-1) {
					G = src_pixels[src_w*(y+1)];
					H = G;
				} else {
					G = D;
					H = D;
				}
			}
			if (x < src_w-1) {
				F = src_pixels[src_w*y+(x+1)];
				if (y > 0) C = src_pixels[src_w*(y-1)+(x+1)];
				else C = src_pixels[src_w*y+(x+1)];
				if (y < src_h-1) I = src_pixels[src_w*(y+1)+(x+1)];
				else I = src_pixels[src_w*y+(x+1)];
			} else { // x==right border
				F = E;
				if (y > 0) C = src_pixels[src_w*(y-1)+x];
				else C = F;
				if (y < src_h-1) I = src_pixels[src_w*(y+1)+x];
				else I = F;
			}
			if (B != H && D != F) {
				E0 = D == B ? D : E;
				E1 = B == F ? F : E;
				E2 = D == H ? D : E;
				E3 = H == F ? F : E;
			} else {
				E0 = E;
				E1 = E;
				E2 = E;
				E3 = E;
			}
			
			int dst_offset = dst->width()*2*y+2*x;
			dst_pixels[dst_offset]     = (E0 << 16) | (E0 << 8) | E0;
			dst_pixels[dst_offset+1]   = (E1 << 16) | (E1 << 8) | E1;
			dst_pixels[dst_offset+320] = (E2 << 16) | (E2 << 8) | E2;
			dst_pixels[dst_offset+321] = (E3 << 16) | (E3 << 8) | E3;
		}
	}
	screen->setPixmap(QPixmap::fromImage(*screenImage));
}

void QtBoiMainWindow::onScalingNone()
{
	scalingMethod = SCALING_NONE;
}
				
void QtBoiMainWindow::onScalingQImage()
{
	scalingMethod = SCALING_QIMAGE;
}

void QtBoiMainWindow::onScalingScale2X()
{
	scalingMethod = SCALING_SCALE2X;
}

void QtBoiMainWindow::onViewDisassemblyWindow()
{
	if (viewDisassemblyWindow->isChecked()) {
		disassembly->show();
	} else {
		disassembly->hide();
	}
}

void QtBoiMainWindow::onViewStatusWindow()
{
	if (viewStatusWindow->isChecked()) {
		status->show();
	} else {
		status->hide();
	}
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


