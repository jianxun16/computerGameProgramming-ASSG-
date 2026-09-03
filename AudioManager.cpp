#include "AudioManager.h"
//set volume 以外的audio东西


AudioManager::AudioManager()
{
    system      = 0;
    sound1      = 0;
    sound2      = 0;
    jumpSfx     = 0;
    slashSfx    = 0;
    bossAttackSfx = 0;
    map1BGM     = 0;
    channel     = 0;
    bgmChannel  = 0;
    bgmGroup    = 0;
    sfxGroup    = 0;

    musicVolume = 1.0f;
    sfxVolume   = 1.0f;
    fadeLevel   = 0.0f;   // silent until fadeInBGM()
    fadeTarget  = 0.0f;
    fadeRate    = 1.0f;
}

AudioManager::~AudioManager()
{
}

void AudioManager::initializeAudio()                // create the FMOD system
{
    result = FMOD::System_Create(&system);
    result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);

    // One group per audio "channel" for independent volume.
    system->createChannelGroup("BGM", &bgmGroup);
    system->createChannelGroup("SFX", &sfxGroup);
    if (sfxGroup) sfxGroup->setVolume(sfxVolume);
    if (bgmGroup) bgmGroup->setVolume(0.0f);

    // Real-time clock for a frame-rate independent fade.
    QueryPerformanceFrequency(&timerFreq);
    QueryPerformanceCounter(&lastTime);
}

void AudioManager::loadSounds()                     // check the asset paths
{
    result = system->createSound("Assets/T3Sahur.wav", FMOD_DEFAULT, 0, &sound1);
    if (sound1) result = sound1->setMode(FMOD_LOOP_OFF);

    result = system->createStream("Assets/67.wav", FMOD_DEFAULT, 0, &sound2);
    if (sound2) result = sound2->setMode(FMOD_LOOP_OFF);

    // One-shot effects.
    result = system->createSound("Assets/SoundEffect/PlayerJump.wav", FMOD_DEFAULT, 0, &jumpSfx);
    if (jumpSfx) result = jumpSfx->setMode(FMOD_LOOP_OFF);

    result = system->createSound("Assets/SoundEffect/SwordSlash.wav", FMOD_DEFAULT, 0, &slashSfx);
    if (slashSfx) result = slashSfx->setMode(FMOD_LOOP_OFF);

    result = system->createSound("Assets/Boss/67_bossAttackSoundEffect.wav", FMOD_DEFAULT, 0, &bossAttackSfx);
    if (bossAttackSfx) result = bossAttackSfx->setMode(FMOD_LOOP_OFF);

    // Looping level music.
    result = system->createStream("Assets/BGM/Map1BGM.wav", FMOD_LOOP_NORMAL, 0, &map1BGM);
    if (map1BGM) result = map1BGM->setMode(FMOD_LOOP_NORMAL);
}

void AudioManager::updateSound()                    // call once a frame
{
    // Seconds since last call, clamped so a long pause can't jump the fade.
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    float dt = (float)(now.QuadPart - lastTime.QuadPart) / (float)timerFreq.QuadPart;
    lastTime = now;
    if (dt < 0.0f)   dt = 0.0f;
    if (dt > 0.1f)   dt = 0.1f;

    // Advance the fade toward its target.
    if (fadeLevel < fadeTarget)
    {
        fadeLevel += fadeRate * dt;
        if (fadeLevel > fadeTarget) fadeLevel = fadeTarget;
    }
    else if (fadeLevel > fadeTarget)
    {
        fadeLevel -= fadeRate * dt;
        if (fadeLevel < fadeTarget) fadeLevel = fadeTarget;
    }
    applyBGMVolume();

    result = system->update();
}

void AudioManager::playJump()                       // PlayerJump.wav via SFX group
{
    // Start paused, raise pitch, then unpause so it never plays at 1.0 first.
    // 1.0 = original, higher = faster/higher-pitched.
    const float JUMP_SPEED = 1.6f;

    FMOD::Channel* ch = 0;
    result = system->playSound(jumpSfx, sfxGroup, true, &ch);
    if (ch)
    {
        ch->setPitch(JUMP_SPEED);
        ch->setPaused(false);
    }
    channel = ch;
}

void AudioManager::playSlash()                      // SwordSlash.wav via SFX group
{
    result = system->playSound(slashSfx, sfxGroup, false, &channel);
}

void AudioManager::playBossAttack()                 // 67_bossAttackSoundEffect.wav via SFX group
{
    result = system->playSound(bossAttackSfx, sfxGroup, false, &channel);
}

void AudioManager::playSound2()                     // (legacy) 67.wav via SFX group
{
    result = system->playSound(sound2, sfxGroup, false, &channel);
}

void AudioManager::playSoundtrack()
{
    result = system->playSound(sound2, sfxGroup, false, &channel);
}

void AudioManager::playMap1BGM()
{
    if (map1BGM == 0) return;

    // Restart cleanly so re-entering doesn't stack channels.
    if (bgmGroup) bgmGroup->stop();
    fadeLevel = 0.0f;                 // silent; fadeInBGM() brings it up
    result = system->playSound(map1BGM, bgmGroup, false, &bgmChannel);
    applyBGMVolume();
}

void AudioManager::stopBGM()
{
    if (bgmGroup) bgmGroup->stop();
    fadeLevel  = 0.0f;
    fadeTarget = 0.0f;
    bgmChannel = 0;
}

void AudioManager::fadeInBGM(float seconds)
{
    fadeTarget = 1.0f;
    fadeRate   = (seconds > 0.0f) ? (1.0f / seconds) : 1000.0f;
}

void AudioManager::fadeOutBGM(float seconds)
{
    fadeTarget = 0.0f;
    fadeRate   = (seconds > 0.0f) ? (1.0f / seconds) : 1000.0f;
}

void AudioManager::setMusicVolume(float v)
{
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;
    musicVolume = v;
    applyBGMVolume();
}

void AudioManager::setSFXVolume(float v)
{
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;
    sfxVolume = v;
    if (sfxGroup) sfxGroup->setVolume(sfxVolume);
}

void AudioManager::applyBGMVolume()
{
    if (bgmGroup) bgmGroup->setVolume(musicVolume * fadeLevel);
}
