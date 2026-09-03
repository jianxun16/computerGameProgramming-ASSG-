#pragma once
#include "GameState.h"
#include <d3d9.h>
#include <d3dx9.h>

// Settings screen pushed from the main menu. Same Music/SFX sliders as the
// pause menu, plus an Exit button (Esc works too). Volume is read/written on the
// shared AudioManager, so both screens always agree.
class SettingState : public GameState
{
public:
    SettingState(Game* game);
    ~SettingState();

    void onEnter() override;
    void onExit()  override;
    void update(InputManager* input) override;
    void render(Graphics* gfx) override;

private:
    // Horizontal 0..1 slider (as in the pause menu).
    struct Slider
    {
        float x, y, w, h;   // track rectangle (screen pixels)
        float value;        // 0..1
        bool  dragging;
    };

    void        drawRect(LPD3DXSPRITE sprite, float x, float y, float w, float h, D3DCOLOR color);
    void        drawSlider(Graphics* gfx, const Slider& s);
    static bool pointIn(int px, int py, float x, float y, float w, float h);
    static bool nearTrack(int px, int py, const Slider& s);   // generous grab

    LPDIRECT3DTEXTURE9 whiteTex;   // 1x1 white, tinted per rect
    ID3DXFont*         font;       // labels / button
    ID3DXFont*         titleFont;  // "SETTINGS"

    Slider bgm;   // Music volume
    Slider sfx;   // SFX volume

    // Last logged volume %, so dragging only logs on a whole-% change.
    int lastMusicPct;
    int lastSfxPct;

    // "Exit" button rect (screen pixels).
    float exitX, exitY, exitW, exitH;
};
