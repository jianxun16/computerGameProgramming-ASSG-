#pragma once
#include <Windows.h>
class FrameTimer
{
	public:
		void Init(int fps);
		int FramesToUpdate();
	private:
		LARGE_INTEGER timer_freq;
		LARGE_INTEGER time_now;
		LARGE_INTEGER time_previous;
		int Requested_FPS;
		float intervalsPerFrame;
	

};

