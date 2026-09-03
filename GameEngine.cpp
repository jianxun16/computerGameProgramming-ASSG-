#include "GameEngine.h"
#include "Cheat.h"
#include <iostream>

using namespace std;
GameEngine::GameEngine() {
    camera = NULL;
    screenWidth = 0;
    screenHeight = 0;
    requestedFPS = 60;
}

GameEngine::~GameEngine() {
    Shutdown();
}

bool GameEngine::Initialize(HINSTANCE hInst, int width, int height, bool fullscreen, int fps) {
    screenWidth = width;
    screenHeight = height;
    requestedFPS = fps;

    // make window
    if (!window.InitializeWindow(GetModuleHandle(NULL), width, height, fullscreen)) return false;
    cout << "initalizing window..\n";

    // start graphics
    if (!graphics.InitializeGraphics(window.GetD3DDevice())) return false;
    cout << "initalizing graphics..\n";

    // get Hinstance for input
    if (!input.InitializeInput(GetModuleHandle(NULL), window.GetWindowHandle())) return false;
    cout << "initalizing input..\n";

    // audio
    audio.InitializeAudio();
    // Preload the game's sounds once so the states can Play() them by key.
    audio.LoadSound("bgm_stage1", "Assets/BGM/Map1BGM.wav", true, true);   // stream + loop
    audio.LoadSound("BossBGM", "Assets/Boss/BossBGM.wav", true, true);     // stream + loop
    audio.LoadSound("JumpSFX", "Assets/SoundEffect/PlayerJump.wav",false,false);
    audio.LoadSound("SlashSFX", "Assets/SoundEffect/SwordSlash.wav",false,false);
    audio.LoadSound("BossAttack", "Assets/Boss/67_bossAttackSoundEffect.wav",false,false);
    cout << "initalizing audio..\n";

    // frame timer
    timer.Init(fps);
    cout << "initalizing timer..\n";

    // cam
    camera = new Camera(width, height);
    cout << "initalizing camera..\n";

    // set stateManager
    stateManager.SetEngine(this);
    cout << "setting engine..\n";
    return true;
}

void GameEngine::Run() {
    const float fixedDt = 1.0f / (float)requestedFPS;

    while (window.ProcessMessages()) {

        // cheat console: type "idkfa" + Enter in the console window to toggle god mode
        Cheat::pollConsole();

        int ticks = timer.FrameToUpdate();
        for (int i = 0; i < ticks; i++) {

            // input
            input.PollDeviceStates();

            // physics and update
            GameState* active = stateManager.GetActiveState();
            if (active) active->UpdateLogic(&input, fixedDt);

            // render the whole stack so overlays (pause / end) sit on the frozen scene
            graphics.BeginRender(0, 0, 0);
            stateManager.RenderAll(&graphics);
            graphics.EndRender();

            // sound
            audio.UpdateSound();

            // state
            stateManager.ApplyPendingTransitions();
        }
        if (camera) camera->Update();


    }
}

void GameEngine::Shutdown() {
    stateManager.CleanUpStates();               

    if (camera) { delete camera; camera = NULL; }

    assets.CleanUpAssets();                     
    audio.CleanUpAudio();                    
    input.CleanUpInput();                  
    graphics.CleanUpGraphics();        
    window.CleanUpWindow();                
}