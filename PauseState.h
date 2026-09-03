#pragma once
#include "GameState.h"
#include <d3d9.h>
#include <d3dx9.h>

// Pause overlay pushed over gameplay: only the top of the stack updates, so the
// game freezes while still rendered below. Has Music/SFX sliders, a Continue
// button (pop, resume) and a Back to Menu button (clear game -> menu).
class PauseState : public GameState
{
public:
    PauseState(Game* game);
    ~PauseState();

    void onEnter() override;
    void onExit() override;
    void update(InputManager* input) override;
    void render(Graphics* gfx) override;

private:
    // A horizontal 0..1 slider.
    struct Slider
    {
        float x, y, w, h;   // track rectangle (screen pixels)
        float value;        // 0..1
        bool  dragging;
    };

    void  drawRect(LPD3DXSPRITE sprite, float x, float y, float w, float h, D3DCOLOR color);
    void  drawSlider(Graphics* gfx, const Slider& s);
    static bool pointIn(int px, int py, float x, float y, float w, float h);
    static bool nearTrack(int px, int py, const Slider& s);   // generous grab

    LPDIRECT3DTEXTURE9 whiteTex;   // 1x1 white, tinted per rect
    ID3DXFont*         font;       // labels / buttons
    ID3DXFont*         titleFont;  // "PAUSED"

    Slider bgm;
    Slider sfx;

    // Last logged volume %, so dragging only logs on a whole-% change.
    int lastMusicPct;
    int lastSfxPct;

    // Button rectangles (screen pixels).
    float continueX, continueY, continueW, continueH;
    float menuX, menuY, menuW, menuH;
};
