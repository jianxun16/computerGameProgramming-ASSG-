#include "MenuState.h"
#include "Game.h"
#include "Graphics.h"
#include "InputManager.h"
#include "PlayState.h"
#include "SettingState.h"
#include "GameLog.h"
#include <Windows.h>

MenuState::MenuState(Game* game) : GameState(game)
{
    background = NULL;
    buttonTex  = NULL;
    font       = NULL;
    titleFont  = NULL;
    btnW = 260.0f;
    btnH = 72.0f;

    ZeroMemory(&playBtn.rect,    sizeof(RECT));
    ZeroMemory(&settingBtn.rect, sizeof(RECT));
    ZeroMemory(&exitBtn.rect,    sizeof(RECT));
    playBtn.label    = "Play";
    settingBtn.label = "Setting";
    exitBtn.label    = "Exit";
}

MenuState::~MenuState()
{
    if (background) { background->Release(); background = NULL; }
    if (buttonTex)  { buttonTex->Release();  buttonTex  = NULL; }
    if (font)       { font->Release();       font       = NULL; }
    if (titleFont)  { titleFont->Release();  titleFont  = NULL; }
}

void MenuState::onEnter()
{
    Graphics* g = game->graphics();
    IDirect3DDevice9* dev = g->device();

    // Background + button art (same paths the rest of the game loads from).
    if (FAILED(D3DXCreateTextureFromFile(dev, "Assets/menu/menu.png", &background)))
        GameLog("MenuState: could not load Assets/menu/menu.png");
    if (FAILED(D3DXCreateTextureFromFile(dev, "Assets/menu/Button1.png", &buttonTex)))
        GameLog("MenuState: could not load Assets/menu/Button1.png");

    D3DXCreateFont(dev, 28, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                   OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                   "Arial", &font);
    D3DXCreateFont(dev, 54, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                   OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                   "Arial", &titleFont);

    layoutButtons((float)g->width(), (float)g->height());

    GameLog("Entered Main Menu");
}

void MenuState::onExit()
{
    if (background) { background->Release(); background = NULL; }
    if (buttonTex)  { buttonTex->Release();  buttonTex  = NULL; }
    if (font)       { font->Release();       font       = NULL; }
    if (titleFont)  { titleFont->Release();  titleFont  = NULL; }
}

void MenuState::layoutButtons(float screenW, float screenH)
{
    const float gap = 26.0f;
    const float x   = (screenW - btnW) / 2.0f;   // centred horizontally
    float y = screenH * 0.42f;                    // first button below the title

    Button* order[3] = { &playBtn, &settingBtn, &exitBtn };
    for (int i = 0; i < 3; i++)
    {
        order[i]->rect.left   = (LONG)x;
        order[i]->rect.top    = (LONG)y;
        order[i]->rect.right  = (LONG)(x + btnW);
        order[i]->rect.bottom = (LONG)(y + btnH);
        y += btnH + gap;
    }
}

bool MenuState::pointIn(int px, int py, const RECT& r)
{
    return px >= r.left && px <= r.right && py >= r.top && py <= r.bottom;
}

void MenuState::update(InputManager* input)
{
    // Enter is a keyboard shortcut for Play.
    if (input->isKeyPressed(DIK_RETURN))
    {
        GameLog("Player clicked Play - starting game (Enter)");
        game->pushState(new PlayState(game));   // start the game, menu stays underneath
        return;
    }

    if (!input->mouseLeftClicked())
        return;

    const int mx = input->mouseX();
    const int my = input->mouseY();

    if (pointIn(mx, my, playBtn.rect))
    {
        GameLog("Player clicked Play - starting game");
        game->pushState(new PlayState(game));    // start the game, menu stays underneath
    }
    else if (pointIn(mx, my, settingBtn.rect))
    {
        GameLog("Player opened Settings");
        game->pushState(new SettingState(game)); // settings on top of the menu
    }
    else if (pointIn(mx, my, exitBtn.rect))
    {
        GameLog("Player clicked Exit - quitting game");
        PostQuitMessage(0);                       // ends the message loop -> window closes
    }
}

void MenuState::drawButton(LPD3DXSPRITE sprite, const Button& b, bool hovered)
{
    if (buttonTex != NULL)
    {
        D3DSURFACE_DESC desc;
        buttonTex->GetLevelDesc(0, &desc);

        float scaleX = btnW / (float)desc.Width;
        float scaleY = btnH / (float)desc.Height;

        D3DXVECTOR2 scaling(scaleX, scaleY);
        D3DXVECTOR2 center(0.0f, 0.0f);
        D3DXVECTOR2 pos((float)b.rect.left, (float)b.rect.top);

        D3DXMATRIX m;
        D3DXMatrixTransformation2D(&m, NULL, 0.0f, &scaling, &center, 0.0f, &pos);
        sprite->SetTransform(&m);
        // Full white when hovered, slightly dimmed otherwise.
        D3DCOLOR tint = hovered ? D3DCOLOR_XRGB(255, 255, 255) : D3DCOLOR_XRGB(205, 205, 205);
        sprite->Draw(buttonTex, NULL, NULL, NULL, tint);

        D3DXMATRIX id; D3DXMatrixIdentity(&id); sprite->SetTransform(&id);
    }

    RECT r = b.rect;
    font->DrawTextA(sprite, b.label, -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP,
                    D3DCOLOR_XRGB(255, 255, 255));
}

void MenuState::render(Graphics* gfx)
{
    LPD3DXSPRITE sp = gfx->sprite();
    const float W = (float)gfx->width();
    const float H = (float)gfx->height();

    // Background stretched to fill the whole window.
    if (background != NULL)
    {
        D3DSURFACE_DESC desc;
        background->GetLevelDesc(0, &desc);

        D3DXVECTOR2 scaling(W / (float)desc.Width, H / (float)desc.Height);
        D3DXVECTOR2 center(0.0f, 0.0f);
        D3DXVECTOR2 pos(0.0f, 0.0f);

        D3DXMATRIX m;
        D3DXMatrixTransformation2D(&m, NULL, 0.0f, &scaling, &center, 0.0f, &pos);
        sp->SetTransform(&m);
        sp->Draw(background, NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));
        D3DXMATRIX id; D3DXMatrixIdentity(&id); sp->SetTransform(&id);
    }

    // Title: "defeat the 67 boss".
    RECT tr;
    tr.left = 0; tr.right = (LONG)W;
    tr.top = (LONG)(H * 0.14f); tr.bottom = (LONG)(H * 0.30f);
    titleFont->DrawTextA(sp, "defeat the 67 boss", -1, &tr,
                         DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP,
                         D3DCOLOR_XRGB(255, 240, 90));

    // Buttons, highlighted while the mouse is over them.
    int mx = 0, my = 0;
    if (game->input()) { mx = game->input()->mouseX(); my = game->input()->mouseY(); }

    drawButton(sp, playBtn,    pointIn(mx, my, playBtn.rect));
    drawButton(sp, settingBtn, pointIn(mx, my, settingBtn.rect));
    drawButton(sp, exitBtn,    pointIn(mx, my, exitBtn.rect));
}
