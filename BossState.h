#pragma once
#include "GameState.h"
#include "TileMap.h"
#include "Background.h"
#include "Player.h"
#include <vector>

// Boss room (Map2.txt), entered from the end of PlayState.
//
// Fight flow:
//   1. The boss (67Boss.png) stays put and AUTO-FIRES two "67" balls at a time,
//      on a repeating timer, aimed at the player. One ball is scale 1.0, the
//      other scale 1.6. They bounce around (gravity + walls + spikes + each
//      other = physics) and live up to 7 seconds each.
//   2. A ball touching the player (or the player walking into a spike) = LOSE.
//   3. The boss has 1 HP: one hit from the player's attack circle = WIN.
class BossState : public GameState {
public:
    BossState();

    void Initialize(GameEngine* eng) override;
    void Resume() override;
    void UpdateLogic(Input* input, float deltaTime) override;
    void RenderFrame(Graphics* graphics) override;

private:
    // One thrown "67" ball: a bouncing circle in WORLD coordinates (centre).
    struct Ball {
        float x, y;     // world-space centre
        float vx, vy;   // velocity (pixels per 1/32 s frame)
        float radius;   // world-space radius (already scaled)
        float scale;    // sprite scale (1.0 or 1.6)
        float mass;     // heavier when bigger (scale^2), used for ball-vs-ball
        float life;     // seconds of life left (starts at BALL_LIFETIME)
        bool  alive;
    };

    // --- Fight helpers ---
    void  fireBallPair();                        // boss auto-fires two balls at the player
    void  updateBalls(float frames);             // one physics step for every ball
    void  stepBall(Ball& b, float frames);       // gravity + tile/wall/spike bounce
    void  resolveBallPair(Ball& a, Ball& b);     // elastic ball-vs-ball collision
    bool  ballHitsPlayer(const Ball& b) const;
    bool  attackHitsBoss() const;                // player attack circle -> boss box
    void  deflectBalls();                        // player attack knocks incoming balls away
    bool  ballBlocked(float cx, float cy, float r) const;
    static bool circleVsBox(float cx, float cy, float r,
                            float left, float top, float right, float bottom);

    // Scene objects (value members, like PlayState).
    TileMap    map;
    Background background;
    Player     player;

    // Boss sprite (67Boss.png) + the projectile sprite (67BossAttack.png).
    LPDIRECT3DTEXTURE9 bossTex;
    LPDIRECT3DTEXTURE9 ballTex;
    float bossX, bossY;   // world position (top-left)
    float bossW, bossH;   // texture size in pixels
    float ballW, ballH;   // texture size in pixels

    bool  bossAlive;      // false once the player lands the killing hit
    bool  ended;          // true once a WIN / LOSE state has been requested
    float fireTimer;      // seconds until the boss fires the next ball pair
    std::vector<Ball> balls;
};
