#pragma once
#include <Windows.h>
#include "fmod.hpp"


// Owns the FMOD system plus two channel groups so the pause menu can control
// music and sound effects independently:
//   bgmGroup  - background music (Map1BGM). Volume = musicVolume * fadeLevel.
//   sfxGroup  - one-shot sound effects (jump, etc.). Volume = sfxVolume.
// updateSound() MUST be called once per frame; it services FMOD and advances
// the BGM fade toward its target.
class AudioManager
{
public:
    FMOD::System        *system;
    FMOD::Sound         *sound1, *sound2;      // (legacy, kept for compatibility)
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

    // ----- Sound effects (routed through sfxGroup, so the SFX slider controls them) -----
    void playJump();                           // PlayerJump.wav  (Space)
    void playSlash();                          // SwordSlash.wav  (left click)
    void playSound2();                         // (legacy)
    void playSoundtrack();

    // ----- Background music -----
    void playMap1BGM();                        // start the looping level music (silent until faded in)
    void stopBGM();                            // stop the music entirely
    void fadeInBGM(float seconds);             // ramp music up to the slider volume
    void fadeOutBGM(float seconds);            // ramp music down to silence (keeps playing)

    // ----- Volume, driven by the pause-menu sliders (0.0 .. 1.0) -----
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
    float          fadeRate;      // units per second (1 / fade duration)

    LARGE_INTEGER  timerFreq;     // for real-time fade steps
    LARGE_INTEGER  lastTime;

    void applyBGMVolume();        // push musicVolume * fadeLevel to bgmGroup
};
