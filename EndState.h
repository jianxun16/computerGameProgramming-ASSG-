#pragma once
#include "GameState.h"
#include <d3d9.h>
#include <d3dx9.h>

// A full-screen end-of-game overlay pushed on top of gameplay. Two flavours,
// chosen at construction:
//   RESULT_WIN  -> "VICTORY"   (green title)
//   RESULT_LOSE -> "GAME OVER" (red title)
// Each shows a single "Back to Menu" button that clears the whole state stack
// back to the MenuState. Like PauseState this is an OVERLAY: because only the
// TOP of the stack updates, the scene underneath freezes, while render still
// draws that frozen scene below it (dimmed).
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

    // "Back to Menu" button rectangle (screen pixels).
    float menuX, menuY, menuW, menuH;
};
