#pragma once
#include "GameState.h"
#include <d3d9.h>
#include <d3dx9.h>

// Full-screen end-of-game overlay: RESULT_WIN ("VICTORY", green) or RESULT_LOSE
// ("GAME OVER", red). A single "Back to Menu" button clears the stack back to
// MenuState. Like PauseState, an overlay that freezes the dimmed scene below.
class EndState : public GameState
{
public:
    enum Result { RESULT_WIN, RESULT_LOSE };

    EndState(Game* game, Result result);
    ~EndState();

    void onEnter() override;
    void onExit() override;
    void update(InputManager* input) override;
    void render(Graphics* gfx) override;

private:
    void  drawRect(LPD3DXSPRITE sprite, float x, float y, float w, float h, D3DCOLOR color);
    static bool pointIn(int px, int py, float x, float y, float w, float h);

    Result             result;
    LPDIRECT3DTEXTURE9 whiteTex;    // 1x1 white, tinted per rect
    ID3DXFont*         font;        // button label
    ID3DXFont*         titleFont;   // "VICTORY" / "GAME OVER"

    // "Back to Menu" button rect (screen pixels).
    float menuX, menuY, menuW, menuH;
};
