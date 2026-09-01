#include "FrameTimer.h"

void FrameTimer::Init(int fps)
{
	QueryPerformanceFrequency(&timerFreq);
	QueryPerformanceCounter(&timeNow);
	QueryPerformanceCounter(&timePrevious);

	//init fps time info
	requestedFPS = fps;

	//number of interval in given
	//timer, per frame at the requested rate.
	intervalsPerFrame = ((float)timerFreq.QuadPart / requestedFPS);
}

int FrameTimer::FrameToUpdate()
{
	framesToUpdate = 0;
	QueryPerformanceCounter(&timeNow);

	//geting delta time
	intervalsSinceLastUpdate = (double)timeNow.QuadPart - timePrevious.QuadPart;

	framesToUpdate = (int)(intervalsSinceLastUpdate / intervalsPerFrame);

	if (framesToUpdate != 0) {
		timePrevious = timeNow;
	}

	return framesToUpdate;
}
