#pragma once
#include "GameState.h"
#include <vector>

using namespace std;

class GameEngine;  
//pending added to avoid instant deletion error
class StateManager {
public:
    StateManager();
    void SetEngine(GameEngine* eng) { engine = eng; }

    void PushState(GameState* state);
    void PopState();
    void ChangeState(GameState* state);   // pop the current, push this one

    void ApplyPendingTransitions();

    GameState* GetActiveState();
    void CleanUpStates();

private:
    enum PendingType { P_PUSH, P_POP, P_CHANGE };
    struct Pending { PendingType type; GameState* state; };

    GameEngine* engine;
    vector<GameState*> stateStack;
    vector<Pending>    pending;
};