#include "FrameTimer.h"

void FrameTimer::Init(int fps)
{
	
	QueryPerformanceFrequency(&timer_freq);
	QueryPerformanceCounter(&time_now);
	QueryPerformanceCounter(&time_previous);
	
	//init fps time info
	Requested_FPS=fps;
	//The number of intervals in the given
	//timer, per frame at the requested rate.
	intervalsPerFrame = ((float)timer_freq.QuadPart / Requested_FPS);
	
	
}
int FrameTimer::FramesToUpdate() {

	int framesToUpdate = 0;
	QueryPerformanceCounter(&time_now);

	//getting the delta time
	
	float intervalsSinceLastUpdate = (float)time_now.QuadPart - (float)time_previous.QuadPart;
	framesToUpdate = (int)(intervalsSinceLastUpdate / intervalsPerFrame);

	if (framesToUpdate != 0) { 
		QueryPerformanceCounter(&time_previous); 
	}

	return framesToUpdate;
}
