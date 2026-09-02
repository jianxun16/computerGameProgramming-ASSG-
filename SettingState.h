#pragma once
#include "GameState.h"

class SettingState : public GameState {
private:
    struct Slider {
        float x, y, w, h;   
        float value;         
        bool dragging;
    };

    Slider bgm;
    Slider sfx; 

    float exitX, exitY, exitW, exitH; 

    bool PointInRect(int px, int py, float x, float y, float w, float h);
    bool NearTrack(int px, int py, const Slider& s);

public:
    void Initialize(GameEngine* eng) override;
    void UpdateLogic(Input* input, float deltaTime) override;
    void RenderFrame(Graphics* graphics) override;
};