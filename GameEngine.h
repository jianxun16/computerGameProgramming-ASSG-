#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "GameWindow.h"
#include "Graphics.h"
#include "Input.h"
#include "AudioManager.h"
#include "AssetManager.h"
#include "FrameTimer.h"
#include "Camera.h"
#include "StateManager.h"

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    bool Initialize(HINSTANCE hInst, int width, int height, bool fullscreen, int fps);
    void Run();
    void Shutdown();

    GameWindow* GetWindow() { return &window; }
    Graphics* GetGraphics() { return &graphics; }
    Input* GetInput() { return &input; }
    AudioManager* GetAudio() { return &audio; }
    AssetManager* GetAssets() { return &assets; }
    Camera* GetCamera() { return camera; }
    StateManager* GetStateManager() { return &stateManager; }

    int GetScreenWidth()  const { return screenWidth; }
    int GetScreenHeight() const { return screenHeight; }
    int GetFPS()          const { return requestedFPS; }

private:
    // Value members are constructed in declaration order and destroyed in reverse. Order matters: window creates the D3D device that graphics uses.
    GameWindow    window;
    Graphics      graphics;
    Input         input;
    AudioManager  audio;
    AssetManager  assets;
    FrameTimer    timer;
    Camera* camera;       
    StateManager  stateManager;

    int screenWidth;
    int screenHeight;
    int requestedFPS;
};