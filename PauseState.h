#pragma once
#include "GameState.h"
#include <d3dx9.h>

class PauseState : public GameState {
private:
    struct Slider {
        float x, y, w, h;
        float value;
        bool dragging;
    };

    LPDIRECT3DTEXTURE9 uiTex;

    Slider bgm;
    Slider sfx;

    float continueX, continueY, continueW, continueH;
    float menuX, menuY, menuW, menuH;

    bool PointInRect(int px, int py, float x, float y, float w, float h);
    bool NearTrack(int px, int py, const Slider& s);
    void DrawUIBox(Graphics* graphics, float x, float y, float w, float h);

public:
    void Initialize(GameEngine* eng) override;
    void UpdateLogic(Input* input, float deltaTime) override;
    void RenderFrame(Graphics* graphics) override;
};