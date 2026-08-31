#pragma once
#include "GameState.h"
#include <d3d9.h>
#include <d3dx9.h>

// A pause overlay pushed on top of the gameplay. Because only the TOP of the
// state stack updates, pushing this freezes the game underneath (time + world
// stop), while render still draws the frozen game below it. Contains:
//   - a Music (BGM) volume slider
//   - a Sound-effect (SFX) volume slider
//   - a "Continue" button  -> pops this overlay, resuming the game
//   - a "Back to Menu" button -> clears the game and returns to the menu
// Music keeps playing while paused so the sliders can be heard as you drag them.
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
    static bool nearTrack(int px, int py, const Slider& s);   // generous grab area

    LPDIRECT3DTEXTURE9 whiteTex;   // 1x1 white, tinted per rect
    ID3DXFont*         font;       // labels / buttons
    ID3DXFont*         titleFont;  // "PAUSED"

    Slider bgm;
    Slider sfx;

    // Last volume percentages written to the CLI log, so dragging only prints
    // a line when the whole-number percentage actually changes.
    int lastMusicPct;
    int lastSfxPct;

    // Button rectangles (screen pixels).
    float continueX, continueY, continueW, continueH;
    float menuX, menuY, menuW, menuH;
};
