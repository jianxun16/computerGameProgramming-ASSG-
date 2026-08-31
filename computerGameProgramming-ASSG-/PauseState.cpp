#include "PauseState.h"
#include "Game.h"
#include "Graphics.h"
#include "InputManager.h"
#include "AudioManager.h"
#include "MenuState.h"
#include "GameLog.h"
#include <cstdio>

// A 1x1 white texture we stretch + tint into any coloured rectangle.
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

PauseState::PauseState(Game* game) : GameState(game)
{
    whiteTex  = NULL;
    font      = NULL;
    titleFont = NULL;

    // Geometry is laid out in onEnter() once we know the screen size.
    bgm.dragging = false;
    sfx.dragging = false;
}

PauseState::~PauseState()
{
    // Safety net for the shutdown path (Game::cleanup deletes without onExit).
    // Null-guarded, so it's harmless if onExit already freed these.
    if (whiteTex)  { whiteTex->Release();  whiteTex  = NULL; }
    if (font)      { font->Release();      font      = NULL; }
    if (titleFont) { titleFont->Release(); titleFont = NULL; }
}

void PauseState::onEnter()
{
    Graphics* g = game->graphics();
    IDirect3DDevice9* dev = g->device();
    const float W = (float)g->width();
    const float H = (float)g->height();

    whiteTex = MakeWhite(dev);

    D3DXCreateFont(dev, 22, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                   OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                   "Arial", &font);
    D3DXCreateFont(dev, 40, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                   OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                   "Arial", &titleFont);

    // Centred panel.
    const float panelW = 440.0f, panelH = 380.0f;
    const float panelX = (W - panelW) / 2.0f;
    const float panelY = (H - panelH) / 2.0f;

    const float inset  = 40.0f;
    const float trackX = panelX + inset;
    const float trackW = panelW - inset * 2.0f;

    bgm.x = trackX;  bgm.y = panelY + 112.0f;  bgm.w = trackW;  bgm.h = 12.0f;
    sfx.x = trackX;  sfx.y = panelY + 182.0f;  sfx.w = trackW;  sfx.h = 12.0f;

    // Start the knobs where the current volumes are.
    bgm.value = game->audio()->getMusicVolume();
    sfx.value = game->audio()->getSFXVolume();
    lastMusicPct = (int)(bgm.value * 100.0f + 0.5f);
    lastSfxPct   = (int)(sfx.value * 100.0f + 0.5f);

    continueX = trackX;  continueY = panelY + 230.0f;  continueW = trackW;  continueH = 44.0f;
    menuX     = trackX;  menuY     = panelY + 288.0f;  menuW     = trackW;  menuH     = 44.0f;
}

void PauseState::onExit()
{
    if (whiteTex)  { whiteTex->Release();  whiteTex  = NULL; }
    if (font)      { font->Release();      font      = NULL; }
    if (titleFont) { titleFont->Release(); titleFont = NULL; }
}

bool PauseState::pointIn(int px, int py, float x, float y, float w, float h)
{
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

bool PauseState::nearTrack(int px, int py, const Slider& s)
{
    const float grab = 16.0f;   // generous band so the knob is easy to grab
    return px >= s.x - grab && px <= s.x + s.w + grab &&
           py >= s.y - grab && py <= s.y + s.h + grab;
}

void PauseState::update(InputManager* input)
{
    // Esc closes the menu (same as Continue).
    if (input->isKeyPressed(DIK_ESCAPE))
    {
        GameLog("Player resumed the game (Esc)");
        game->popState();
        return;
    }

    const int  mx      = input->mouseX();
    const int  my      = input->mouseY();
    const bool down    = input->mouseLeftDown();
    const bool clicked = input->mouseLeftClicked();

    // ----- Sliders -----
    if (!down)
    {
        bgm.dragging = false;
        sfx.dragging = false;
    }
    else
    {
        // Grab a slider on the click that lands on its track.
        if (clicked)
        {
            if      (nearTrack(mx, my, bgm)) bgm.dragging = true;
            else if (nearTrack(mx, my, sfx)) sfx.dragging = true;
        }

        if (bgm.dragging)
        {
            float v = (mx - bgm.x) / bgm.w;
            if (v < 0.0f) v = 0.0f; if (v > 1.0f) v = 1.0f;
            bgm.value = v;
            game->audio()->setMusicVolume(v);
            int pct = (int)(v * 100.0f + 0.5f);
            if (pct != lastMusicPct) { GameLog("Player set Music volume to %d%%", pct); lastMusicPct = pct; }
        }
        if (sfx.dragging)
        {
            float v = (mx - sfx.x) / sfx.w;
            if (v < 0.0f) v = 0.0f; if (v > 1.0f) v = 1.0f;
            sfx.value = v;
            game->audio()->setSFXVolume(v);
            int pct = (int)(v * 100.0f + 0.5f);
            if (pct != lastSfxPct) { GameLog("Player set Sound Effect volume to %d%%", pct); lastSfxPct = pct; }
        }
    }

    // ----- Buttons (act on the click release-free edge) -----
    if (clicked)
    {
        if (pointIn(mx, my, continueX, continueY, continueW, continueH))
        {
            GameLog("Player resumed the game (Continue)");
            game->popState();          // resume the game underneath
            return;
        }
        if (pointIn(mx, my, menuX, menuY, menuW, menuH))
        {
            GameLog("Player returned to Menu (game cleared)");
            game->audio()->stopBGM();               // silence the level music
            game->setRootState(new MenuState(game)); // clear the game -> menu
            return;
        }
    }
}

void PauseState::drawRect(LPD3DXSPRITE sprite, float x, float y, float w, float h, D3DCOLOR color)
{
    D3DXVECTOR2 scaling(w, h);          // 1x1 texture -> w x h pixels
    D3DXVECTOR2 trans(x, y);
    D3DXMATRIX m;
    D3DXMatrixTransformation2D(&m, NULL, 0.0f, &scaling, NULL, 0.0f, &trans);
    sprite->SetTransform(&m);
    D3DXVECTOR3 origin(0.0f, 0.0f, 0.0f);
    sprite->Draw(whiteTex, NULL, NULL, &origin, color);
}

void PauseState::drawSlider(Graphics* gfx, const Slider& s)
{
    LPD3DXSPRITE sp = gfx->sprite();
    float knobX = s.x + s.value * s.w;

    drawRect(sp, s.x, s.y, s.w, s.h, D3DCOLOR_ARGB(255, 70, 74, 90));        // track
    drawRect(sp, s.x, s.y, knobX - s.x, s.h, D3DCOLOR_ARGB(255, 90, 200, 120)); // fill
    drawRect(sp, knobX - 7.0f, s.y - 6.0f, 14.0f, s.h + 12.0f,               // knob
             D3DCOLOR_ARGB(255, 240, 240, 245));
}

void PauseState::render(Graphics* gfx)
{
    LPD3DXSPRITE sp = gfx->sprite();
    const float W = (float)gfx->width();
    const float H = (float)gfx->height();

    const float panelW = 440.0f, panelH = 380.0f;
    const float panelX = (W - panelW) / 2.0f;
    const float panelY = (H - panelH) / 2.0f;

    // Dim the frozen game, then draw the panel.
    drawRect(sp, 0.0f, 0.0f, W, H, D3DCOLOR_ARGB(150, 0, 0, 0));
    drawRect(sp, panelX, panelY, panelW, panelH, D3DCOLOR_ARGB(235, 28, 32, 46));

    // Sliders.
    drawSlider(gfx, bgm);
    drawSlider(gfx, sfx);

    // Buttons (lighter when hovered).
    int mx = 0, my = 0;
    if (game->input()) { mx = game->input()->mouseX(); my = game->input()->mouseY(); }
    bool hovCont = pointIn(mx, my, continueX, continueY, continueW, continueH);
    bool hovMenu = pointIn(mx, my, menuX, menuY, menuW, menuH);
    drawRect(sp, continueX, continueY, continueW, continueH,
             hovCont ? D3DCOLOR_ARGB(255, 90, 200, 120) : D3DCOLOR_ARGB(255, 60, 130, 85));
    drawRect(sp, menuX, menuY, menuW, menuH,
             hovMenu ? D3DCOLOR_ARGB(255, 200, 90, 90) : D3DCOLOR_ARGB(255, 130, 60, 60));

    // Rectangles done -> clear the sprite transform before drawing text.
    D3DXMATRIX id; D3DXMatrixIdentity(&id); sp->SetTransform(&id);

    // ----- Text -----
    // Draw through the SHARED sprite (already Begun with ALPHABLEND in
    // Graphics::beginFrame). Passing NULL here makes the font spin up its own
    // sprite nested inside the active batch, and the text never shows.
    RECT r;
    auto setR = [&](int l, int t, int rr, int b) { r.left = l; r.top = t; r.right = rr; r.bottom = b; };

    // Title.
    setR((int)panelX, (int)(panelY + 24.0f), (int)(panelX + panelW), (int)(panelY + 74.0f));
    titleFont->DrawTextA(sp,"PAUSED", -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                         D3DCOLOR_ARGB(255, 240, 240, 245));

    char buf[64];

    // Music label + percentage.
    sprintf_s(buf, "Music   %d%%", (int)(bgm.value * 100.0f + 0.5f));
    setR((int)bgm.x, (int)(bgm.y - 34.0f), (int)(bgm.x + bgm.w), (int)(bgm.y - 8.0f));
    font->DrawTextA(sp,buf, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE,
                    D3DCOLOR_ARGB(255, 220, 220, 228));

    // SFX label + percentage.
    sprintf_s(buf, "Sound Effect   %d%%", (int)(sfx.value * 100.0f + 0.5f));
    setR((int)sfx.x, (int)(sfx.y - 34.0f), (int)(sfx.x + sfx.w), (int)(sfx.y - 8.0f));
    font->DrawTextA(sp,buf, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE,
                    D3DCOLOR_ARGB(255, 220, 220, 228));

    // Button labels.
    setR((int)continueX, (int)continueY, (int)(continueX + continueW), (int)(continueY + continueH));
    font->DrawTextA(sp,"Continue", -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                    D3DCOLOR_ARGB(255, 255, 255, 255));

    setR((int)menuX, (int)menuY, (int)(menuX + menuW), (int)(menuY + menuH));
    font->DrawTextA(sp,"Back to Menu", -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                    D3DCOLOR_ARGB(255, 255, 255, 255));
}
