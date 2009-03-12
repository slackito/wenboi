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


QtBoiEmuThread::QtBoiEmuThread(QObject *parent, QString romName)
	:QThread(parent)
{
	gb = new GameBoy(romName.toStdString());
	isPaused = true;
	quitRequested = false;
	frameCount = 0;
}

QtBoiEmuThread::~QtBoiEmuThread()
{
	stop();
	wait();
	delete gb;
}

void QtBoiEmuThread::toggle_paused()
{
	isPaused = !isPaused;
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
	cout << "Running! \\o/" << endl;
	while(!quitRequested)
	{
		if(isPaused)
			usleep(100);
		else {
			switch (runningMode)
			{
				case RUN:
					while (!isPaused && 
							(status == GameBoy::NORMAL || status == GameBoy::WAIT))
					{
						status = gb->run_cycle();
						if (gb->video.get_frames_rendered() > frameCount)
						{
							frameCount = gb->video.get_frames_rendered();
							emit redraw(gb->video.get_screen_buffer());
						}
					}
					break;
				case STEP:
					do
					{
						status = gb->run_cycle();
						if (gb->video.get_frames_rendered() > frameCount)
						{
							frameCount = gb->video.get_frames_rendered();
							emit redraw(gb->video.get_screen_buffer());
						}
					} while(status == GameBoy::WAIT); // do nothing
					
					break;
			}

			print_run_result(*gb, status);
			isPaused=true;
			emit emulationPaused(); 
		}
	}
}


