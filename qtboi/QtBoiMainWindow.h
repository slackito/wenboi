#ifndef QTBOIMAINWINDOW_H
#define QTBOIMAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QKeyEvent>
#include <QActionGroup>
#include <QPushButton>
#include <QImage>
#include <QLabel>
#include <QStatusBar>
#include <QTextBrowser>
#include <QString>
#include <QUrl>
#include <QHash>

#include "QtBoiEmuThread.h"
#include "QtBoiDisassemblyWindow.h"
#include "QtBoiStatusWindow.h"

class QtBoiMainWindow: public QMainWindow
{
	Q_OBJECT

	public:
		QtBoiMainWindow(QWidget *parent=0);
		~QtBoiMainWindow();

	public slots:
		void onLoadROM();
		void onDisassemblyAnchorClicked(const QUrl&);
		void onRedraw(const uchar *buffer);
		void onPause();
		void onResume();
		void onScalingNone();
		void onScalingQImage();
		void onScalingScale2X();
		void onViewDisassemblyWindow();
		void onViewStatusWindow();
        void onDebugVideoDrawBackground();
        void onDebugVideoDrawWindow();
        void onDebugVideoDrawSprites();

	private:
		enum ScalingMethod {SCALING_NONE, SCALING_QIMAGE, SCALING_SCALE2X};
		ScalingMethod scalingMethod;

		// private functions
		void scale2x(const QImage *src, QImage *dst);

		void createActions();
		void createMenu();
		void createToolbar();

		void loadTags();
		void saveTags();

        double now(); // in seconds

		// events
		void keyPressEvent(QKeyEvent *event);
		void keyReleaseEvent(QKeyEvent *event);

        // attributes
        unsigned int frames_since_last_FPS_update;
        float last_FPS_update;
        unsigned int init_seconds;

		QtBoiEmuThread *emuThread;
		
		QString romTitle;
		QHash<u32, QString> tags;

		QWidget *centralWindow;
		QLabel  *screen;
		QImage  *screenImage;
		QImage  *scaledImage;

        QStatusBar *statusbar;
        QtBoiDisassemblyWindow *disassembly;
		QtBoiStatusWindow *status;

		QAction *loadROM;
		QAction *quit;
		QAction *emulatorPause;
		QAction *emulatorCont;
		QAction *emulatorStop;
		QAction *emulatorStep;
		QAction *emulatorReset;

		QAction *viewDisassemblyWindow;
		QAction *viewStatusWindow;

		QActionGroup *scalingGroup;
		QAction *scalingNone;
		QAction *scalingQImage;
		QAction *scalingScale2X;

        QAction *debugVideoDrawBackground;
        QAction *debugVideoDrawWindow;
        QAction *debugVideoDrawSprites;
};


#endif


