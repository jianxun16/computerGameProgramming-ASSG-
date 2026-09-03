#pragma once
#include "Graphics.h"
#include "Input.h"

class GameEngine;   

// abstract class. DO NOT INITIALISE
class GameState {
public:
    virtual ~GameState() {}

    GameEngine* engine;

    virtual void Initialize(GameEngine* eng) { engine = eng; }

    //pause for pushing new item and pausing the one below
    virtual void Pause() {}
    virtual void Resume() {}

    // Pure virtual: every state must implement these.
    virtual void UpdateLogic(Input* input, float deltaTime) = 0;
    virtual void RenderFrame(Graphics* graphics) = 0;
};