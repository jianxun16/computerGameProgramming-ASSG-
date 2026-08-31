#pragma once

class Game;
class InputManager;
class Graphics;

// Base class for everything that can sit on the game's state stack
// (MenuState, PlayState, later PauseState...). Only the top of the stack is
// updated; render walks the whole stack bottom -> top so overlays can show
// what's underneath.
class GameState
{
public:
    GameState(Game* game) : game(game) {}
    virtual ~GameState() {}

    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void onResume() {}   // called when this state becomes the top again after a pop
    virtual void update(InputManager* input) {}
    virtual void render(Graphics* gfx) {}

protected:
    Game* game;   // to reach shared services and request push/pop
};
