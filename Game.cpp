#include "Game.h"
#include "GameState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "FrameTimer.h"
#include "AudioManager.h"

// ----- TEMP SWITCH -----
// The UI (MenuState) is wired up now, so boot on the menu.
//   false -> boot straight into the game, skipping the menu
//   true  -> start on the menu (current)
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

    // Pick the first state based on the switch above.
    if (START_IN_MENU)
        pushState(new MenuState(this));
    else
        pushState(new PlayState(this));   // skip the menu for now
    applyPending();     // apply the initial push right away

    return true;
}

void Game::run()
{
    while (window.processMessages())
    {
        inp.update();
        update();
        audioManager->updateSound();   // service FMOD + advance BGM fade, once a frame
        render();
        applyPending();   // apply any push/pop requested this frame
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
        states.back()->update(&inp);   // only the top of the stack runs
        inp.postUpdate();              // consume input edges for this step
    }
}

void Game::render()
{
    gfx.beginFrame();

    // Draw bottom -> top so an overlay (e.g. a pause menu) shows the state
    // beneath it. Empty states simply draw nothing.
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
    // Wipe every current state (e.g. "back to menu" clearing the game) and make
    // `state` the fresh root. Applied at the end of the frame like push/pop.
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
            // Tear the whole stack down, then install the new root.
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
            // Never pop the last state (that would leave a blank screen).
            // e.g. when the menu is switched off, Backspace in PlayState is
            // ignored instead of emptying the stack.
            if (states.size() > 1)
            {
                states.back()->onExit();
                delete states.back();
                states.pop_back();
                states.back()->onResume();   // the revealed state is top again
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
