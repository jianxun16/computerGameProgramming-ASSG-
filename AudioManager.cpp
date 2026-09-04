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
    FMOD::System_Create(&system);

    system->setDSPBufferSize(512, 4);
    system->init(512, FMOD_INIT_NORMAL, NULL);

    system->createChannelGroup("BGM", &bgmGroup);
    system->createChannelGroup("SFX", &sfxGroup);

    bgmVolume = 1.0f;
    sfxVolume = 1.0f;
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


void AudioManager::PlayPitchSFX(const string& key, float pitch) {
    auto it = soundMap.find(key);                   
    if (it == soundMap.end() || it->second == NULL) return;

    FMOD::Channel* ch = NULL;
    system->playSound(it->second, sfxGroup, true, &ch); // pause first
    if (ch) {
        ch->setPitch(pitch);      
        ch->setPaused(false);     //continue
    }
}

void AudioManager::PlaySFXAt(const string& key,
    float lx, float ly, float sx, float sy,
    float minDist, float maxDist) {            

    auto it = soundMap.find(key);
    if (it == soundMap.end() || it->second == NULL) return;

    float dx = lx - sx, dy = ly - sy;
    float dist = sqrtf(dx * dx + dy * dy);

    float volume;
    if (dist <= minDist) 
    {
        volume = 1.0f;
    }
    else if (dist >= maxDist) 
    {
        volume = 0.0f;
    }
    else 
    {
        volume = 1.0f - (dist - minDist) / (maxDist - minDist); 
    }

    FMOD::Channel* ch = NULL;
    system->playSound(it->second, sfxGroup, true, &ch);
    if (ch) {
        ch->setVolume(volume);
        ch->setPaused(false);                 
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