#include "StateManager.h"

void StateManager::PushState(GameState* state) {
    stateStack.push_back(state);
    stateStack.back()->Initialize();
}

void StateManager::PopState() {
    if (!stateStack.empty()) {
        delete stateStack.back();
        stateStack.pop_back();
    }
}

void StateManager::ChangeState(GameState* state) {
    CleanUpStates();
    PushState(state);
}

GameState* StateManager::GetActiveState() {
    if (stateStack.empty()) return nullptr;
    return stateStack.back();
}

void StateManager::CleanUpStates() {
    for (GameState* state : stateStack) {
        delete state;
    }
    stateStack.clear();
}