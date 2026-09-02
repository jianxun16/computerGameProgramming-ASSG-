#include "StateManager.h"

StateManager::StateManager() {
    engine = NULL;
}

void StateManager::PushState(GameState* state) {
    Pending p; p.type = P_PUSH; p.state = state;
    pending.push_back(p);
}

void StateManager::PopState() {
    Pending p; p.type = P_POP; p.state = NULL;
    pending.push_back(p);
}

void StateManager::ChangeState(GameState* state) {
    Pending p; p.type = P_CHANGE; p.state = state;
    pending.push_back(p);
}

void StateManager::ApplyPendingTransitions() {
    for (size_t i = 0; i < pending.size(); i++) {
        Pending& p = pending[i];

        if (p.type == P_PUSH) {
            // pause then push
            if (!stateStack.empty()) {
                stateStack.back()->Pause();
            }
            stateStack.push_back(p.state);
            stateStack.back()->Initialize(engine);
        }
        else if (p.type == P_POP) {
            // destroy and resume
            if (!stateStack.empty()) {
                delete stateStack.back();
                stateStack.pop_back();
            }
            if (!stateStack.empty()) {
                stateStack.back()->Resume();
            }
        }
        else { // change: pop then push
            if (!stateStack.empty()) {
                delete stateStack.back();
                stateStack.pop_back();
            }
            stateStack.push_back(p.state);
            stateStack.back()->Initialize(engine);
        }
    }
    pending.clear();
}

GameState* StateManager::GetActiveState() {
    if (stateStack.empty()) return NULL;
    return stateStack.back();
}

void StateManager::CleanUpStates() {
    for (GameState* state : stateStack) {
        delete state;
    }
    stateStack.clear();
    pending.clear();
}