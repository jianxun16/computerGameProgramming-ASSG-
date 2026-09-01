#include "SettingState.h"
#include "Game.h"
#include "Graphics.h"
#include "InputManager.h"
#include "AudioManager.h"
#include "GameLog.h"
#include <cstdio>

// A 1x1 white texture we stretch + tint into any coloured rectangle.
// (Same helper the pause menu uses.)
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

SettingState::SettingState(Game* game) : GameState(game)
{
    whiteTex     = NULL;
    font         = NULL;
    titleFont    = NULL;
    lastMusicPct = 0;
    lastSfxPct   = 0;
    exitX = exitY = exitW = exitH = 0.0f;

    bgm.dragging = false;
    sfx.dragging = false;
}

SettingState::~SettingState()
{
    // Safety net for the shutdown path (states may be deleted without onExit).
    if (whiteTex)  { whiteTex->Release();  whiteTex  = NULL; }
    if (font)      { font->Release();      font      = NULL; }
    if (titleFont) { titleFont->Release(); titleFont = NULL; }
}

void SettingState::onEnter()
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
    const float panelW = 440.0f, panelH = 340.0f;
    const float panelX = (W - panelW) / 2.0f;
    const float panelY = (H - panelH) / 2.0f;

    const float inset  = 40.0f;
    const float trackX = panelX + inset;
    const float trackW = panelW - inset * 2.0f;

    bgm.x = trackX;  bgm.y = panelY + 112.0f;  bgm.w = trackW;  bgm.h = 12.0f;
    sfx.x = trackX;  sfx.y = panelY + 182.0f;  sfx.w = trackW;  sfx.h = 12.0f;

    // Start the knobs where the CURRENT (shared) volumes already are, so this
    // screen and the pause menu always show the same values.
    bgm.value = game->audio()->getMusicVolume();
    sfx.value = game->audio()->getSFXVolume();
    lastMusicPct = (int)(bgm.value * 100.0f + 0.5f);
    lastSfxPct   = (int)(sfx.value * 100.0f + 0.5f);

    // "Exit" button along the bottom of the panel.
    exitW = trackW;  exitH = 44.0f;
    exitX = trackX;  exitY = panelY + panelH - exitH - 28.0f;

    GameLog("Entered Settings");
}

void SettingState::onExit()
{
    if (whiteTex)  { whiteTex->Release();  whiteTex  = NULL; }
    if (font)      { font->Release();      font      = NULL; }
    if (titleFont) { titleFont->Release(); titleFont = NULL; }
}

bool SettingState::pointIn(int px, int py, float x, float y, float w, float h)
{
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

bool SettingState::nearTrack(int px, int py, const Slider& s)
{
    const float grab = 16.0f;   // generous band so the knob is easy to grab
    return px >= s.x - grab && px <= s.x + s.w + grab &&
           py >= s.y - grab && py <= s.y + s.h + grab;
}

void SettingState::update(InputManager* input)
{
    // Esc closes Settings (same as pressing Exit).
    if (input->isKeyPressed(DIK_ESCAPE))
    {
        GameLog("Player closed Settings (Esc)");
        game->popState();
        return;
    }

    const int  mx      = input->mouseX();
    const int  my      = input->mouseY();
    const bool down    = input->mouseLeftDown();
    const bool clicked = input->mouseLeftClicked();

    // ----- Sliders (identical to the pause menu) -----
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
            game->audio()->setMusicVolume(v);          // shared with the pause menu
            int pct = (int)(v * 100.0f + 0.5f);
            if (pct != lastMusicPct) { GameLog("Player set Music volume to %d%%", pct); lastMusicPct = pct; }
        }
        if (sfx.dragging)
        {
            float v = (mx - sfx.x) / sfx.w;
            if (v < 0.0f) v = 0.0f; if (v > 1.0f) v = 1.0f;
            sfx.value = v;
            game->audio()->setSFXVolume(v);            // shared with the pause menu
            int pct = (int)(v * 100.0f + 0.5f);
            if (pct != lastSfxPct) { GameLog("Player set Sound volume to %d%%", pct); lastSfxPct = pct; }
        }
    }

    // ----- Exit button -----
    if (clicked && pointIn(mx, my, exitX, exitY, exitW, exitH))
    {
        GameLog("Player closed Settings");
        game->popState();
        return;
    }
}

void SettingState::drawRect(LPD3DXSPRITE sprite, float x, float y, float w, float h, D3DCOLOR color)
{
    D3DXVECTOR2 scaling(w, h);          // 1x1 texture -> w x h pixels
    D3DXVECTOR2 trans(x, y);
    D3DXMATRIX m;
    D3DXMatrixTransformation2D(&m, NULL, 0.0f, &scaling, NULL, 0.0f, &trans);
    sprite->SetTransform(&m);
    D3DXVECTOR3 origin(0.0f, 0.0f, 0.0f);
    sprite->Draw(whiteTex, NULL, NULL, &origin, color);
}

void SettingState::drawSlider(Graphics* gfx, const Slider& s)
{
    LPD3DXSPRITE sp = gfx->sprite();
    float knobX = s.x + s.value * s.w;

    drawRect(sp, s.x, s.y, s.w, s.h, D3DCOLOR_ARGB(255, 70, 74, 90));            // track
    drawRect(sp, s.x, s.y, knobX - s.x, s.h, D3DCOLOR_ARGB(255, 90, 200, 120));  // fill
    drawRect(sp, knobX - 7.0f, s.y - 6.0f, 14.0f, s.h + 12.0f,                   // knob
             D3DCOLOR_ARGB(255, 240, 240, 245));
}

void SettingState::render(Graphics* gfx)
{
    LPD3DXSPRITE sp = gfx->sprite();
    const float W = (float)gfx->width();
    const float H = (float)gfx->height();

    const float panelW = 440.0f, panelH = 340.0f;
    const float panelX = (W - panelW) / 2.0f;
    const float panelY = (H - panelH) / 2.0f;

    // Dim the menu behind, then draw the panel.
    drawRect(sp, 0.0f, 0.0f, W, H, D3DCOLOR_ARGB(160, 0, 0, 0));
    drawRect(sp, panelX, panelY, panelW, panelH, D3DCOLOR_ARGB(235, 28, 32, 46));

    // Sliders.
    drawSlider(gfx, bgm);
    drawSlider(gfx, sfx);

    // Exit button (lighter when hovered).
    int mx = 0, my = 0;
    if (game->input()) { mx = game->input()->mouseX(); my = game->input()->mouseY(); }
    bool hovExit = pointIn(mx, my, exitX, exitY, exitW, exitH);
    drawRect(sp, exitX, exitY, exitW, exitH,
             hovExit ? D3DCOLOR_ARGB(255, 200, 90, 90) : D3DCOLOR_ARGB(255, 130, 60, 60));

    // Rectangles done -> clear the sprite transform before drawing text.
    D3DXMATRIX id; D3DXMatrixIdentity(&id); sp->SetTransform(&id);

    // ----- Text ----- (drawn through the SHARED sprite that Graphics already
    // Begun with ALPHABLEND; passing NULL would nest a second batch and the
    // text would not show.)
    RECT r;
    auto setR = [&](int l, int t, int rr, int b) { r.left = l; r.top = t; r.right = rr; r.bottom = b; };

    // Title.
    setR((int)panelX, (int)(panelY + 24.0f), (int)(panelX + panelW), (int)(panelY + 74.0f));
    titleFont->DrawTextA(sp, "SETTINGS", -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                         D3DCOLOR_ARGB(255, 240, 240, 245));

    char buf[64];

    // Music label + percentage.
    sprintf_s(buf, "Music   %d%%", (int)(bgm.value * 100.0f + 0.5f));
    setR((int)bgm.x, (int)(bgm.y - 34.0f), (int)(bgm.x + bgm.w), (int)(bgm.y - 8.0f));
    font->DrawTextA(sp, buf, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE,
                    D3DCOLOR_ARGB(255, 220, 220, 228));

    // SFX label + percentage.
    sprintf_s(buf, "Sound   %d%%", (int)(sfx.value * 100.0f + 0.5f));
    setR((int)sfx.x, (int)(sfx.y - 34.0f), (int)(sfx.x + sfx.w), (int)(sfx.y - 8.0f));
    font->DrawTextA(sp, buf, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE,
                    D3DCOLOR_ARGB(255, 220, 220, 228));

    // Exit button label.
    setR((int)exitX, (int)exitY, (int)(exitX + exitW), (int)(exitY + exitH));
    font->DrawTextA(sp, "Exit", -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                    D3DCOLOR_ARGB(255, 255, 255, 255));
}
