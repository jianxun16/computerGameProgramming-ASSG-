#pragma once
#include "fmod.hpp"
#include <string>
#include <unordered_map>
#pragma comment(lib, "fmod_vc.lib")

using namespace std;

class AudioManager {
public:
    enum SoundType { SFX, BGM };

private:
    FMOD::System* system;
    unordered_map<string, FMOD::Sound*> soundMap;
    FMOD::Channel* channel;      
    FMOD::Channel* bgmChannel;   
    FMOD::ChannelGroup* bgmGroup;
    FMOD::ChannelGroup* sfxGroup;
    FMOD_RESULT result;

    float bgmVolume;   
    float sfxVolume; 

public:
    AudioManager();

    void InitializeAudio();

    // stream for bgm
    void LoadSound(const string& key, const string& filePath,
        bool isStream = false, bool loop = false);
    // for sfx
    void Play(const string& key, SoundType type = SFX);

    void PlayPitchSFX(const string& key, float pitch);

    void StopBGM();

    void  SetMusicVolume(float v);   // 0..1
    void  SetSFXVolume(float v);     // 0..1
    float GetMusicVolume() const { return bgmVolume; }
    float GetSFXVolume()   const { return sfxVolume; }


    void UpdateSound();
    void CleanUpAudio();
};