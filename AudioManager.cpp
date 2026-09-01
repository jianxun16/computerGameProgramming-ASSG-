#include "AudioManager.h"

void AudioManager::InitializeAudio()
{
	result = FMOD::System_Create(&system);
	result = system->init(32, FMOD_INIT_NORMAL, extradrivedata);
}

void AudioManager::PlaySound1()
{
	result = system->playSound(sound1, 0, false, &channel);
}

void AudioManager::PlaySoundtrack()
{
	result = system->playSound(sound2, 0, false, &channel);
}

void AudioManager::LoadSounds()
{
	result = system->createSound("", FMOD_DEFAULT, 0, &sound1);
	result = sound1->setMode(FMOD_LOOP_OFF);

	result = system->createStream("", FMOD_DEFAULT, 0, &sound2);
	result = sound2->setMode(FMOD_LOOP_OFF);
}

void AudioManager::UpdateSound()
{
	result = system->update();
}
