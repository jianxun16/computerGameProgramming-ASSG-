#include "StateManager.h"

void StateManager::PushState(GameState* state) {
    // pause before push
    if (!stateStack.empty()) {
        stateStack.back()->Pause();
    }

    // push and point
    stateStack.push_back(state);
    stateStack.back()->Initialize(this);
}

void StateManager::PopState() {
    // destroy top
    if (!stateStack.empty()) {
        delete stateStack.back();
        stateStack.pop_back();
    }

    // wake top
    if (!stateStack.empty()) {
        stateStack.back()->Resume();
    }
}

void StateManager::ChangeState(GameState* state) {
    // destroy and push
    if (!stateStack.empty()) {
        delete stateStack.back();
        stateStack.pop_back();
    }

    stateStack.push_back(state);
    stateStack.back()->Initialize(this);
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