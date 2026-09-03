#include "AudioManager.h"

using namespace std;

AudioManager::AudioManager() {
    system = NULL;
    channel = NULL;
    bgmChannel = NULL;
    bgmGroup = NULL;
    sfxGroup = NULL;
    bgmVolume = 1.0f;
    sfxVolume = 1.0f;
}

void AudioManager::InitializeAudio() {
    result = FMOD::System_Create(&system);
    result = system->init(32, FMOD_INIT_NORMAL, 0);

    system->createChannelGroup("BGM", &bgmGroup);
    system->createChannelGroup("SFX", &sfxGroup);
    if (bgmGroup) bgmGroup->setVolume(bgmVolume);
    if (sfxGroup) sfxGroup->setVolume(sfxVolume);

    channel = NULL;
}

void AudioManager::LoadSound(const string& key, const string& filePath, bool isStream, bool loop) {
    // Don't reload a key that's already loaded (avoids leaking the old FMOD sound).
    if (soundMap.find(key) != soundMap.end()) return;

    FMOD::Sound* newSound = NULL;

    if (isStream) {
        // stream for BGM
        result = system->createStream(filePath.c_str(), FMOD_DEFAULT, 0, &newSound);
    }
    else {
        // sound for SFX
        result = system->createSound(filePath.c_str(), FMOD_DEFAULT, 0, &newSound);
    }

    if (newSound) {
        newSound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
        soundMap[key] = newSound;
    }
}

void AudioManager::Play(const string& key, SoundType type) {
    auto it = soundMap.find(key);
    if (it == soundMap.end() || it->second == NULL) return;

    if (type == BGM) {
        if (bgmGroup) bgmGroup->stop();
        system->playSound(it->second, bgmGroup, false, &bgmChannel);
        channel = bgmChannel;
    }
    else {
        system->playSound(it->second, sfxGroup, false, &channel);
    }
}

void AudioManager::StopBGM() {
    if (bgmGroup) bgmGroup->stop();
    bgmChannel = NULL;
}

void AudioManager::SetMusicVolume(float v) {
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;
    bgmVolume = v;
    if (bgmGroup) bgmGroup->setVolume(v);
}

void AudioManager::SetSFXVolume(float v) {
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;
    sfxVolume = v;
    if (sfxGroup) sfxGroup->setVolume(v);
}

void AudioManager::UpdateSound() {
    system->update();
}

void AudioManager::CleanUpAudio() {
    // Release sounds -> groups -> system, in that order.
    for (auto const& pair : soundMap) {
        if (pair.second) {
            pair.second->release();
        }
    }
    soundMap.clear();

    if (bgmGroup) { bgmGroup->release(); bgmGroup = NULL; }
    if (sfxGroup) { sfxGroup->release(); sfxGroup = NULL; }

    if (system) {
        system->close();
        system->release();
        system = NULL;
    }
}