#include "EndState.h"
#include "Game.h"
#include "Graphics.h"
#include "InputManager.h"
#include "AudioManager.h"
#include "MenuState.h"
#include "GameLog.h"

// A 1x1 white texture we stretch + tint into any coloured rectangle.
// (Same trick PauseState uses.)
static LPDIRECT3DTEXTURE9 MakeWhite(IDirect3DDevice9* device)
{
    LPDIRECT3DTEXTURE9 tex = NULL;
    if (FAILED(device->CreateTexture(1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex, NULL)))
        return NULL;
    D3DLOCKED_RECT lr;
    tex->LockRect(0, &lr, NULL, 0);
    *(DWORD*)lr.pBits = 0xFFFFFFFF;   // opaque white
    tex->UnlockRect(0);
    return tex;
}

EndState::EndState(Game* game, Result result) : GameState(game)
{
    this->result = result;
    whiteTex  = NULL;
    font      = NULL;
    titleFont = NULL;
    // Button geometry is laid out in onEnter() once we know the screen size.
}

EndState::~EndState()
{
    // Safety net for the shutdown path (Game::cleanup deletes without onExit).
    if (whiteTex)  { whiteTex->Release();  whiteTex  = NULL; }
    if (font)      { font->Release();      font      = NULL; }
    if (titleFont) { titleFont->Release(); titleFont = NULL; }
}

void EndState::onEnter()
{
    Graphics* g = game->graphics();
    IDirect3DDevice9* dev = g->device();
    const float W = (float)g->width();
    const float H = (float)g->height();

    whiteTex = MakeWhite(dev);

    D3DXCreateFont(dev, 22, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                   OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                   "Arial", &font);
    D3DXCreateFont(dev, 48, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                   OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                   "Arial", &titleFont);

    // Centred panel with the title on top and one button below.
    const float panelW = 440.0f, panelH = 260.0f;
    const float panelX = (W - panelW) / 2.0f;
    const float panelY = (H - panelH) / 2.0f;

    const float inset = 40.0f;
    menuW = panelW - inset * 2.0f;
    menuH = 48.0f;
    menuX = panelX + inset;
    menuY = panelY + panelH - inset - menuH;   // sits near the bottom of the panel
}

void EndState::onExit()
{
    if (whiteTex)  { whiteTex->Release();  whiteTex  = NULL; }
    if (font)      { font->Release();      font      = NULL; }
    if (titleFont) { titleFont->Release(); titleFont = NULL; }
}

bool EndState::pointIn(int px, int py, float x, float y, float w, float h)
{
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

void EndState::update(InputManager* input)
{
    const int  mx      = input->mouseX();
    const int  my      = input->mouseY();
    const bool clicked = input->mouseLeftClicked();

    // Esc or the button both go back to the menu.
    if (input->isKeyPressed(DIK_ESCAPE) ||
        (clicked && pointIn(mx, my, menuX, menuY, menuW, menuH)))
    {
        GameLog("Player returned to Menu (game cleared)");
        game->audio()->stopBGM();                 // silence any leftover music
        game->setRootState(new MenuState(game));  // clear the game -> menu
        return;
    }
}

void EndState::drawRect(LPD3DXSPRITE sprite, float x, float y, float w, float h, D3DCOLOR color)
{
    D3DXVECTOR2 scaling(w, h);          // 1x1 texture -> w x h pixels
    D3DXVECTOR2 trans(x, y);
    D3DXMATRIX m;
    D3DXMatrixTransformation2D(&m, NULL, 0.0f, &scaling, NULL, 0.0f, &trans);
    sprite->SetTransform(&m);
    D3DXVECTOR3 origin(0.0f, 0.0f, 0.0f);
    sprite->Draw(whiteTex, NULL, NULL, &origin, color);
}

void EndState::render(Graphics* gfx)
{
    LPD3DXSPRITE sp = gfx->sprite();
    const float W = (float)gfx->width();
    const float H = (float)gfx->height();

    const float panelW = 440.0f, panelH = 260.0f;
    const float panelX = (W - panelW) / 2.0f;
    const float panelY = (H - panelH) / 2.0f;

    const bool     win        = (result == RESULT_WIN);
    const char*    title      = win ? "VICTORY" : "GAME OVER";
    const D3DCOLOR titleColor = win ? D3DCOLOR_ARGB(255, 120, 220, 140)
                                    : D3DCOLOR_ARGB(255, 220, 90, 90);

    // Dim the frozen scene, then draw the panel.
    drawRect(sp, 0.0f, 0.0f, W, H, D3DCOLOR_ARGB(160, 0, 0, 0));
    drawRect(sp, panelX, panelY, panelW, panelH, D3DCOLOR_ARGB(235, 28, 32, 46));

    // Button (lighter when hovered).
    int mx = 0, my = 0;
    if (game->input()) { mx = game->input()->mouseX(); my = game->input()->mouseY(); }
    bool hovMenu = pointIn(mx, my, menuX, menuY, menuW, menuH);
    drawRect(sp, menuX, menuY, menuW, menuH,
             hovMenu ? D3DCOLOR_ARGB(255, 90, 140, 210) : D3DCOLOR_ARGB(255, 60, 95, 150));

    // Rectangles done -> clear the sprite transform before drawing text.
    D3DXMATRIX id; D3DXMatrixIdentity(&id); sp->SetTransform(&id);

    // ----- Text ----- (drawn through the shared, already-begun sprite)
    RECT r;
    auto setR = [&](int l, int t, int rr, int b) { r.left = l; r.top = t; r.right = rr; r.bottom = b; };

    // Title.
    setR((int)panelX, (int)(panelY + 40.0f), (int)(panelX + panelW), (int)(panelY + 120.0f));
    titleFont->DrawTextA(sp, title, -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE, titleColor);

    // Button label.
    setR((int)menuX, (int)menuY, (int)(menuX + menuW), (int)(menuY + menuH));
    font->DrawTextA(sp, "Back to Menu", -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                    D3DCOLOR_ARGB(255, 255, 255, 255));
}
