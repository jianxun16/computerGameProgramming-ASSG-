#pragma once
#include "Graphics.h"
#include "Input.h"

class StateManager; 

// abstract class. DO NOT INITIALISE
class GameState {
public:
    virtual ~GameState() {}
    StateManager* stateManager;

    virtual void Initialize(StateManager* manager) { stateManager = manager; }

    virtual void Pause() {}
    virtual void Resume() {}

    // 0 to make sure it must be extended from
    virtual void UpdateLogic(Input* input, float deltaTime) = 0;
    virtual void RenderFrame(Graphics* graphics) = 0;
};