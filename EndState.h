#pragma once
#include "GameState.h"

// Full-screen end-of-game overlay: RESULT_WIN ("VICTORY", green) or RESULT_LOSE
// ("GAME OVER", red). A single "Back to Menu" button pops the whole game stack
// back down to the root MenuState. Like PauseState, an overlay that freezes the
// dimmed scene below it.
class EndState : public GameState {
public:
    enum Result { RESULT_WIN, RESULT_LOSE };

    EndState(Result result) { this->result = result; }

    void Initialize(GameEngine* eng) override;
    void UpdateLogic(Input* input, float deltaTime) override;
    void RenderFrame(Graphics* graphics) override;

private:
    static bool PointInRect(int px, int py, float x, float y, float w, float h);

    Result result;

    // "Back to Menu" button rect (screen pixels), laid out in Initialize.
    float menuX, menuY, menuW, menuH;
};
