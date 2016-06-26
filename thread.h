#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include "render.h"

class Thread:public QThread
{
private:
	Render *render;
	int numThread,totalThread;

public:
	Thread(){}

	void setThread(Render *_render,int _numThread,int _totalThread)
	{
		render=_render;
		numThread=_numThread;
		totalThread=_totalThread;
	}

	void run()
	{
		render->trace(numThread,totalThread);
	}
};

#endif // THREAD_H
