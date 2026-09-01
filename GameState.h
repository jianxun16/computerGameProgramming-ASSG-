#pragma once

class Game;
class InputManager;
class Graphics;

// Base class for anything on the state stack (Menu/Play/Pause...). Only the top
// updates; render walks bottom -> top so overlays show what's underneath.
class GameState
{
public:
    GameState(Game* game) : game(game) {}
    virtual ~GameState() {}

    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void onResume() {}   // becomes the top again after a pop
    virtual void update(InputManager* input) {}
    virtual void render(Graphics* gfx) {}

protected:
    Game* game;   // shared services + push/pop
};
