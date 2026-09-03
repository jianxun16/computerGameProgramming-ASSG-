#pragma once
#include <vector>
#include "GameWindow.h"
#include "Graphics.h"
#include "InputManager.h"

class GameState;
class FrameTimer;
class AudioManager;

// Top-level owner: window, graphics, input, audio, timer, and the state stack.
// main() just calls init() then run().
class Game
{
public:
    Game();
    ~Game();

    bool init();
    void run();
    void cleanup();

    // State stack. push/pop deferred to end of frame, so a state can request a
    // transition from inside its own update().
    void pushState(GameState* state);
    void popState();
    void setRootState(GameState* state);   // clear the stack, make this the only state

    // Shared services handed to states.
    Graphics*     graphics() { return &gfx; }
    InputManager* input()    { return &inp; }
    AudioManager* audio()    { return audioManager; }

private:
    void update();
    void render();
    void applyPending();

    GameWindow    window;
    Graphics      gfx;
    InputManager  inp;
    FrameTimer*   timer;
    AudioManager* audioManager;

    std::vector<GameState*> states;

    // Deferred stack transitions.
    enum PendingType { P_PUSH, P_POP, P_CLEAR };
    struct Pending { PendingType type; GameState* state; };
    std::vector<Pending> pending;

    int screenW;
    int screenH;
};
