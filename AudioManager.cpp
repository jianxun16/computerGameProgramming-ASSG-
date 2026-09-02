#include "AudioManager.h"

using namespace std;

void AudioManager::InitializeAudio() {
    result = FMOD::System_Create(&system);
    result = system->init(32, FMOD_INIT_NORMAL, 0);
    channel = nullptr;
}

void AudioManager::LoadSound(const string& key, const string& filePath, bool isStream) {
    FMOD::Sound* newSound = nullptr;

    if (isStream) {
        // Use createStream for background music / large files
        result = system->createStream(filePath.c_str(), FMOD_DEFAULT, 0, &newSound);
    }
    else {
        // Use createSound for short sound effects (loads fully into memory)
        result = system->createSound(filePath.c_str(), FMOD_DEFAULT, 0, &newSound);
    }

    if (newSound) {
        newSound->setMode(FMOD_LOOP_OFF);
        soundMap[key] = newSound; // Store it safely in our dictionary map
    }
}

void AudioManager::Play(const string& key) {
    auto it = soundMap.find(key);
    if (it != soundMap.end() && it->second != nullptr) {
        system->playSound(it->second, 0, false, &channel);
    }
}

void AudioManager::UpdateSound() {
    system->update();
}

void AudioManager::CleanUpAudio() {
    for (auto const& pair : soundMap) {
        if (pair.second) {
            pair.second->release();
        }
    }
    soundMap.clear();

    if (system) {
        system->close();
        system->release();
        system = nullptr;
    }
}