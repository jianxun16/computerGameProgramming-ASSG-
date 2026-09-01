#pragma once
#include "GameState.h"
#include <d3d9.h>
#include <d3dx9.h>

// The main menu (sits on the state stack). A full-screen background plus a
// title ("defeat the 67 boss") and three image buttons drawn with a label:
//   Play    -> pushes PlayState on top (the menu stays underneath, so Backspace
//              in the game can pop back here)
//   Setting -> pushes SettingState on top of the menu
//   Exit    -> quits the game (ends the message loop, the window closes)
// Buttons are clicked with the mouse; Enter is also a shortcut for Play.
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
    // One clickable image button: its screen rectangle plus the text on it.
    struct Button
    {
        RECT        rect;
        const char* label;
    };

    void        layoutButtons(float screenW, float screenH);
    void        drawButton(LPD3DXSPRITE sprite, const Button& b, bool hovered);
    static bool pointIn(int px, int py, const RECT& r);

    LPDIRECT3DTEXTURE9 background;   // menu.png, stretched to fill the window
    LPDIRECT3DTEXTURE9 buttonTex;    // Button1.png, reused for every button
    ID3DXFont*         font;         // button labels
    ID3DXFont*         titleFont;    // "defeat the 67 boss"

    Button playBtn;
    Button settingBtn;
    Button exitBtn;

    float btnW;
    float btnH;
};
