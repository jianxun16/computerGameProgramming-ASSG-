#include "MenuState.h"
#include "Game.h"
#include "InputManager.h"
#include "PlayState.h"

MenuState::MenuState(Game* game) : GameState(game)
{
}

void MenuState::onEnter()
{
    // TODO: load menu UI (title, "Start" button, background) here.
}

void MenuState::update(InputManager* input)
{
    // TODO: real menu navigation. For now, Enter starts the game:
    // push PlayState on top of this menu (the menu stays underneath).
    if (input->isKeyDown(DIK_RETURN))
        game->pushState(new PlayState(game));
}

void MenuState::render(Graphics* gfx)
{
    // UI intentionally empty for now -> just the cleared background shows.
}
