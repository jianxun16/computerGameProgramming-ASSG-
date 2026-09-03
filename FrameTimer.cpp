#include "FrameTimer.h"

void FrameTimer::Init(int fps)
{
	
	QueryPerformanceFrequency(&timer_freq);
	QueryPerformanceCounter(&time_now);
	QueryPerformanceCounter(&time_previous);
	
	Requested_FPS=fps;
	//timer intervals per frame at the requested rate
	intervalsPerFrame = ((float)timer_freq.QuadPart / Requested_FPS);
	
	
}
int FrameTimer::FramesToUpdate() {

	int framesToUpdate = 0;
	QueryPerformanceCounter(&time_now);

	//delta time
	float intervalsSinceLastUpdate = (float)time_now.QuadPart - (float)time_previous.QuadPart;
	framesToUpdate = (int)(intervalsSinceLastUpdate / intervalsPerFrame);

	if (framesToUpdate != 0) { 
		QueryPerformanceCounter(&time_previous); 
	}

	return framesToUpdate;
}
