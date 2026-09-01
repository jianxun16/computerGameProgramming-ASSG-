#pragma once
#include "GameState.h"
#include <d3d9.h>
#include <d3dx9.h>

// The Settings screen (sits on the state stack, pushed from the main menu).
// It reuses the same sliders as the pause menu:
//   - a Music (BGM) volume slider
//   - a Sound-effect (SFX) volume slider
//   - an "Exit" button (Esc also works) that returns to the main menu
//
// Volume is read from / written to the SHARED AudioManager (game->audio()), the
// same object the pause menu uses. So the two screens always agree: drag Music
// to 80% here and the pause menu shows 80% too, and the other way around, for
// both music and all sound.
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
    // A horizontal 0..1 slider (copied from the pause menu).
    struct Slider
    {
        float x, y, w, h;   // track rectangle (screen pixels)
        float value;        // 0..1
        bool  dragging;
    };

    void        drawRect(LPD3DXSPRITE sprite, float x, float y, float w, float h, D3DCOLOR color);
    void        drawSlider(Graphics* gfx, const Slider& s);
    static bool pointIn(int px, int py, float x, float y, float w, float h);
    static bool nearTrack(int px, int py, const Slider& s);   // generous grab area

    LPDIRECT3DTEXTURE9 whiteTex;   // 1x1 white, tinted per rectangle
    ID3DXFont*         font;       // labels / button
    ID3DXFont*         titleFont;  // "SETTINGS"

    Slider bgm;   // Music volume
    Slider sfx;   // Sound-effect volume

    // Last percentages written to the CLI log, so dragging only prints a line
    // when the whole-number percentage actually changes.
    int lastMusicPct;
    int lastSfxPct;

    // "Exit" button rectangle (screen pixels).
    float exitX, exitY, exitW, exitH;
};
