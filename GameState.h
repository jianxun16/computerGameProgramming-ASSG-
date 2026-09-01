#pragma once
#include "Graphics.h"
#include "Input.h"

// abstract class. DO NOT INITIALISE
class GameState {
public:
    virtual ~GameState() {}

    virtual void Initialize();
    virtual void UpdateLogic(Input* input, float deltaTime) = 0;
    virtual void RenderFrame(Graphics* graphics) = 0;
};