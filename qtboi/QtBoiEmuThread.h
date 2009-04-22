#ifndef QTBOIEMUTHREAD_H
#define QTBOIEMUTHREAD_H

#include <QThread>
#include <QString>
#include "../core/GameBoy.h"

class QtBoiEmuThread: public QThread
{
  Q_OBJECT

  public:
    GameBoy gb;
    GameBoy::run_status status;
    bool isPaused;
    unsigned int frameCount;

    QtBoiEmuThread(QObject *parent);
    ~QtBoiEmuThread();

    void loadROM(QString romName);
 
    void pressControl(GameBoy::Control);
    void releaseControl(GameBoy::Control);

  public slots:
    void reset();
    void run();
    void stop();
    void pause();
    void cont();
    void step();
  
  signals:
    void emulationPaused();
    void redraw(const uchar *buffer);

  private:
    enum RunningMode { RUN, STEP };
    
    QString romName;
    RunningMode runningMode;
    bool quitRequested;
    bool resetRequested;
    bool romLoaded;
};



#endif


