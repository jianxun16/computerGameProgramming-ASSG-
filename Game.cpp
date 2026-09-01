#include "Game.h"
#include "GameState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "FrameTimer.h"
#include "AudioManager.h"

// Boot state: true = menu, false = straight into the game.
static const bool START_IN_MENU = true;

Game::Game()
{
    timer = NULL;
    audioManager = NULL;
    screenW = 800;
    screenH = 600;
}

Game::~Game()
{
    cleanup();
}

bool Game::init()
{
    window.create("Direct3D_9_Animation", screenW, screenH);
    gfx.init(window.handle(), screenW, screenH);
    inp.init(window.handle());

    timer = new FrameTimer();
    timer->Init(32);

    audioManager = new AudioManager();
    audioManager->initializeAudio();
    audioManager->loadSounds();

    // First state.
    if (START_IN_MENU)
        pushState(new MenuState(this));
    else
        pushState(new PlayState(this));
    applyPending();     // apply the initial push now

    return true;
}

void Game::run()
{
    while (window.processMessages())
    {
        inp.update();                  // 1. Input
        update();                      // 2. Physics + Update
        render();                      // 3. Render
        audioManager->updateSound();   // 4. Sound
        applyPending();   // apply this frame's push/pop
    }
}

void Game::update()
{
    if (states.empty())
        return;

    for (int i = 0; i < timer->FramesToUpdate(); i++)
    {
        if (states.empty())
            break;
        states.back()->update(&inp);   // only the top runs
        inp.postUpdate();              // consume input edges
    }
}

void Game::render()
{
    gfx.beginFrame();

    // Draw bottom -> top so overlays show the state beneath.
    for (size_t i = 0; i < states.size(); i++)
        states[i]->render(&gfx);

    gfx.endFrame();
}

void Game::pushState(GameState* state)
{
    Pending p; p.type = P_PUSH; p.state = state;
    pending.push_back(p);
}

void Game::popState()
{
    Pending p; p.type = P_POP; p.state = NULL;
    pending.push_back(p);
}

void Game::setRootState(GameState* state)
{
    // Wipe the stack and make `state` the fresh root (deferred like push/pop).
    Pending p; p.type = P_CLEAR; p.state = state;
    pending.push_back(p);
}

void Game::applyPending()
{
    for (size_t i = 0; i < pending.size(); i++)
    {
        if (pending[i].type == P_PUSH)
        {
            states.push_back(pending[i].state);
            states.back()->onEnter();
        }
        else if (pending[i].type == P_CLEAR)
        {
            // Tear down the stack, then install the new root.
            for (size_t s = 0; s < states.size(); s++)
            {
                states[s]->onExit();
                delete states[s];
            }
            states.clear();
            states.push_back(pending[i].state);
            states.back()->onEnter();
        }
        else // P_POP
        {
            // Never pop the last state (would leave a blank screen).
            if (states.size() > 1)
            {
                states.back()->onExit();
                delete states.back();
                states.pop_back();
                states.back()->onResume();   // revealed state is top again
            }
        }
    }
    pending.clear();
}

void Game::cleanup()
{
    for (size_t i = 0; i < states.size(); i++)
        delete states[i];
    states.clear();
    pending.clear();

    if (audioManager != NULL) { delete audioManager; audioManager = NULL; }
    if (timer != NULL)        { delete timer;        timer = NULL; }

    inp.release();
    gfx.release();
    window.release();
}
