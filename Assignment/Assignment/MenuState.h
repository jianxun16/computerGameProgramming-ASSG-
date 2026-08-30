#pragma once
#include "GameState.h"

// The UI / main-menu state. Intentionally empty for now (blank screen);
// press Enter to start the game. Fill in real UI here later.
class MenuState : public GameState
{
public:
    MenuState(Game* game);

    void onEnter() override;
    void update(InputManager* input) override;
    void render(Graphics* gfx) override;
};
