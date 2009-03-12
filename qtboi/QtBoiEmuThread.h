#ifndef QTBOIEMUTHREAD_H
#define QTBOIEMUTHREAD_H

#include <QThread>
#include <QString>
#include "../core/GameBoy.h"

class QtBoiEmuThread: public QThread
{
  Q_OBJECT

  public:
    GameBoy::run_status status;
    bool isPaused;
    int frameCount;
    QtBoiEmuThread(QObject *parent, QString romName);
    ~QtBoiEmuThread();
 
  public slots:
    void run();
    void stop();
    void toggle_paused();
    void cont();
    void step();
  
  signals:
    void emulationPaused();
    void redraw(const uchar *buffer);

  private:
    enum RunningMode { RUN, STEP };
    GameBoy *gb;
    RunningMode runningMode;
    bool quitRequested;
};



#endif


