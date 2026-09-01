#pragma once
#include "GameState.h"
#include <d3d9.h>
#include <d3dx9.h>

// Main menu: full-screen background, title, and three image buttons. Play pushes
// PlayState (menu stays underneath), Setting pushes SettingState, Exit quits.
// Buttons are mouse-clicked; Enter is a shortcut for Play.
class MenuState : public GameState
{
public:
    MenuState(Game* game);
    ~MenuState();

    void onEnter()  override;
    void onExit()   override;
    void update(InputManager* input) override;
    void render(Graphics* gfx) override;

private:
    // One clickable image button: screen rect + label.
    struct Button
    {
        RECT        rect;
        const char* label;
    };

    void        layoutButtons(float screenW, float screenH);
    void        drawButton(LPD3DXSPRITE sprite, const Button& b, bool hovered);
    static bool pointIn(int px, int py, const RECT& r);

    LPDIRECT3DTEXTURE9 background;   // menu.png, stretched to fill
    LPDIRECT3DTEXTURE9 buttonTex;    // Button1.png, reused per button
    ID3DXFont*         font;         // button labels
    ID3DXFont*         titleFont;    // "defeat the 67 boss"

    Button playBtn;
    Button settingBtn;
    Button exitBtn;

    float btnW;
    float btnH;
};
