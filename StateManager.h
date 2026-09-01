#pragma once
#include "GameState.h"
#include <vector>

using namespace std;

class StateManager {
private:
    vector<GameState*> stateStack;

public:
    void PushState(GameState* state);
    void PopState();
    void ChangeState(GameState* state); //pop delete push

    GameState* GetActiveState();

    void CleanUpStates();
};