#include "GameEngine.h"

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
    if (!window.InitializeWindow(hInst, width, height, fullscreen)) return false;

    // start graphics
    if (!graphics.InitializeGraphics(window.GetD3DDevice())) return false;

    // get Hinstance for input
    if (!input.InitializeInput(hInst, window.GetWindowHandle())) return false;

    // audio
    audio.InitializeAudio();

    // frame timer
    timer.Init(fps);

    // cam
    camera = new Camera(width, height);

    // set stateManager
    stateManager.SetEngine(this);

    return true;
}

void GameEngine::Run() {
    const float fixedDt = 1.0f / (float)requestedFPS;

    while (window.ProcessMessages()) {

        // input
        input.PollDeviceStates();

        // physics and update
        int ticks = timer.FrameToUpdate();
        for (int i = 0; i < ticks; i++) {
            GameState* active = stateManager.GetActiveState();
            if (active) active->UpdateLogic(&input, fixedDt);
        }
        if (camera) camera->Update();

        // render
        graphics.BeginRender(0, 0, 0);
        GameState* active = stateManager.GetActiveState();
        if (active) active->RenderFrame(&graphics);
        graphics.EndRender();

        // sound
        audio.UpdateSound();

        // state
        stateManager.ApplyPendingTransitions();
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