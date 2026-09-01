#pragma once
#include <Windows.h>
#include "fmod.hpp"


// Owns the FMOD system plus two channel groups controlled independently:
//   bgmGroup - music, volume = musicVolume * fadeLevel.
//   sfxGroup - one-shot effects, volume = sfxVolume.
// updateSound() must run once per frame (services FMOD, advances the fade).
class AudioManager
{
public:
    FMOD::System        *system;
    FMOD::Sound         *sound1, *sound2;      // (legacy)
    FMOD::Sound         *jumpSfx;              // PlayerJump.wav  (Space)
    FMOD::Sound         *slashSfx;             // SwordSlash.wav  (left click)
    FMOD::Sound         *map1BGM;              // looping level music
    FMOD::Channel       *channel;              // last SFX channel
    FMOD::Channel       *bgmChannel;           // current music channel
    FMOD::ChannelGroup  *bgmGroup;
    FMOD::ChannelGroup  *sfxGroup;
    FMOD_RESULT          result;
    void                *extradriverdata = 0;

    void initializeAudio();
    void loadSounds();
    void updateSound();                        // call once a frame

    // ----- Sound effects (via sfxGroup) -----
    void playJump();                           // PlayerJump.wav  (Space)
    void playSlash();                          // SwordSlash.wav  (left click)
    void playSound2();                         // (legacy)
    void playSoundtrack();

    // ----- Background music -----
    void playMap1BGM();                        // start looping music (silent until faded in)
    void stopBGM();                            // stop music
    void fadeInBGM(float seconds);             // ramp up to slider volume
    void fadeOutBGM(float seconds);            // ramp down to silence (keeps playing)

    // ----- Volume (0.0 .. 1.0) -----
    void  setMusicVolume(float v);
    void  setSFXVolume(float v);
    float getMusicVolume() const { return musicVolume; }
    float getSFXVolume()   const { return sfxVolume; }

    AudioManager();
    ~AudioManager();

private:
    float          musicVolume;   // BGM slider value (0..1)
    float          sfxVolume;     // SFX slider value (0..1)
    float          fadeLevel;     // current fade multiplier (0..1)
    float          fadeTarget;    // where the fade is heading (0 or 1)
    float          fadeRate;      // units/sec (1 / fade duration)

    LARGE_INTEGER  timerFreq;     // real-time fade steps
    LARGE_INTEGER  lastTime;

    void applyBGMVolume();        // musicVolume * fadeLevel -> bgmGroup
};
