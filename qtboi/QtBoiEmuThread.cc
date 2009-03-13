#include "QtBoiEmuThread.h"
#include <iostream>

using std::cout;
using std::endl;

void print_run_result(GameBoy &gb, int status)
{

	if (status == GameBoy::BREAKPOINT)
	{
		cout << "Breakpoint reached " << endl;
		cout << gb.status_string() << endl;
	}
	else if (status == GameBoy::WATCHPOINT)
	{
		cout << "Watchpoint reached" << endl;
		
		cout << "Watchpoint 0x" << std::hex << std::setw(4) << std::setfill('0') <<
			int(gb.memory.watchpoint_addr) << " hit at 0x" << gb.regs.PC;
		
		// FIXME: Move/expose this things somewhere
		if (gb.memory.watchpoint_newvalue == 0xFFFF)
		{
			cout << " (READ)" << endl << 
				"value = " << int(gb.memory.watchpoint_oldvalue) << endl;
		}
		else
		{
			cout << " (WRITE)" << endl <<
				"old = " << int(gb.memory.watchpoint_oldvalue) << 
				" new = " << int(gb.memory.watchpoint_newvalue) << endl;
		}
	}
	else
	{
		cout << "run returned with status " << status << endl;
		cout << gb.status_string() << endl;
	}
}


QtBoiEmuThread::QtBoiEmuThread(QObject *parent)
	:QThread(parent),
	status(GameBoy::NORMAL),
	isPaused(true),
	frameCount(0),
	romName(),
	runningMode(RUN),
	quitRequested(false),
	resetRequested(false),
	romLoaded(false)
{
}

QtBoiEmuThread::~QtBoiEmuThread()
{
}

void QtBoiEmuThread::loadROM(QString name)
{
	romName=name;
	reset();
	romLoaded=true;
}


void QtBoiEmuThread::pressControl(GameBoy::Control c)
{
	gb.push_control(c);
}

void QtBoiEmuThread::releaseControl(GameBoy::Control c)
{
	gb.release_control(c);
}

void QtBoiEmuThread::reset()
{
	resetRequested=true;
}

void QtBoiEmuThread::pause()
{
	isPaused = true;
}

void QtBoiEmuThread::cont()
{
	status = GameBoy::NORMAL;
	runningMode = RUN;
	isPaused = false;
}

void QtBoiEmuThread::stop()
{
	quitRequested = true;
	isPaused      = true;
}

void QtBoiEmuThread::step()
{
	runningMode = STEP;
	isPaused = false;
}

void QtBoiEmuThread::run()
{
	cout << "emu thread running" << endl;
	
	// wait until ROM is loaded
	while(!romLoaded && !quitRequested)
		msleep(500);

	while(!quitRequested)
	{
		if(resetRequested)
		{
			resetRequested = false;
			gb.load_ROM(romName.toStdString());
			cout << "Loaded ROM " << romName.toStdString() << endl;
		}

		if(isPaused)
			msleep(10);
		else 
		{
			switch (runningMode)
			{
				case RUN:
					while (!isPaused && !resetRequested &&
						(status == GameBoy::NORMAL || status == GameBoy::WAIT))
					{
						status = gb.run_cycle();
						if (gb.video.get_frames_rendered() > frameCount)
						{
							frameCount = gb.video.get_frames_rendered();
							emit redraw(gb.video.get_screen_buffer());
							msleep(5);
						}
					}
					break;
				case STEP:
					do
					{
						status = gb.run_cycle();
						if (gb.video.get_frames_rendered() > frameCount)
						{
							frameCount = gb.video.get_frames_rendered();
							emit redraw(gb.video.get_screen_buffer());
						}
					} while(status == GameBoy::WAIT); // do nothing
					isPaused = true;
					
					break;
			}

			print_run_result(gb, status);
			if (isPaused)
			{
				emit emulationPaused(); 
			}
		}
	}
	
	cout << "Exiting emulation thread" << endl;

}


