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

        // Guard against the "spiral of death". If the game stalls for even a
        // moment (a heavier frame when an overlay is pushed, a slow state
        // Initialize / LoadFont, an alt-tab, a breakpoint), FrameToUpdate returns
        // a big backlog of ticks. The old loop rendered inside this catch-up loop,
        // and every render blocks on the vsync Present -- so a large backlog kept
        // the loop busy for seconds, the Windows message pump never ran, and the
        // window went "Not Responding" (the boss-room pause / victory freeze).
        // Cap the catch-up so we can never fall into that hole.
        const int MAX_TICKS = 5;
        if (ticks > MAX_TICKS) ticks = MAX_TICKS;

        // ---- Update only: fixed timestep, run a few times to catch up. ----
        for (int i = 0; i < ticks; i++) {
            input.PollDeviceStates();

            GameState* active = stateManager.GetActiveState();
            if (active) active->UpdateLogic(&input, fixedDt);

            // Apply queued pushes/pops right away so each tick sees a settled stack.
            stateManager.ApplyPendingTransitions();
        }

        audio.UpdateSound();
        if (camera) camera->Update();

        // ---- Render exactly ONCE per frame, OUTSIDE the catch-up loop. ----
        // Present blocks on vsync, so it must never sit inside the catch-up loop.
        // The whole stack is drawn bottom -> top so overlays (pause / end) sit on
        // the frozen scene below them.
        graphics.BeginRender(0, 0, 0);
        stateManager.RenderAll(&graphics);
        graphics.EndRender();
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