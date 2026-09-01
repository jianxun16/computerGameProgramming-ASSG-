#pragma once
#include "fmod.hpp"

class AudioManager
{
public:
	FMOD::System* system;
	FMOD::Sound* sound1, * sound2;
	FMOD::Channel* channel = 0;
	FMOD_RESULT result;
	void* extradrivedata = 0;

	void InitializeAudio();
	void PlaySound1();
	void PlaySoundtrack();
	void LoadSounds();
	void UpdateSound();

};
