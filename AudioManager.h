#pragma once
#include "fmod.hpp"
#include <string>
#include <unordered_map>

using namespace std; 

class AudioManager {
private:
    FMOD::System* system;
    unordered_map<string, FMOD::Sound*> soundMap;
    FMOD::Channel* channel;
    FMOD_RESULT result;

public:
    void InitializeAudio();

    // give name and path
    void LoadSound(const string& key, const string& filePath, bool isStream = false);
    // play sound by name
    void Play(const string& key);
    void UpdateSound();
    void CleanUpAudio();
};