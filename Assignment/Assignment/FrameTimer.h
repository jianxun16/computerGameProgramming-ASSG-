#pragma once //include guard,
#include <Windows.h>//cant inlcude the same inlcude<> in 1 class
class FrameTimer
{
	//FrameTimer.h
	
	public:
		void Init(int fps);//need return type
		int FramesToUpdate();
	private:
		LARGE_INTEGER timer_freq;
		LARGE_INTEGER time_now;
		LARGE_INTEGER time_previous;
		int Requested_FPS;
		float intervalsPerFrame;
	

};

