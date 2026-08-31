#pragma once
class GameState;          
class InputManager;
class Graphics;

class StateManager
{
public:
    void push(GameState* s);
    void pop();
    void update(InputManager* in);
    void render(Graphics* g);
    void applyPending();
};