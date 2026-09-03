// author by limzhenxing

#pragma once
#include "GameState.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>

class Background;
class PlayerAnimation;
class TileMap;

// Boss room (Map2.txt), entered from the end of PlayState.
//
// Fight flow:
//   1. The boss (67Boss.png) stays put and AUTO-FIRES two "67" balls at a time,
//      on a repeating timer, aimed at the player. One ball is scale 1.0, the
//      other scale 1.6. They bounce around (gravity + walls + spikes + each
//      other = physics) and live up to 7 seconds each.
//   2. A ball touching the player (or the player walking into a spike) = LOSE.
//   3. The boss has 1 HP: one hit from the player's attack circle = WIN.
class BossState : public GameState
{
public:
    BossState(Game* game);
    ~BossState();

    void onEnter() override;
    void update(InputManager* input) override;
    void render(Graphics* gfx) override;

private:
    // One thrown "67" ball: a bouncing circle in WORLD coordinates (centre).
    struct Ball
    {
        float x, y;     // world-space centre
        float vx, vy;   // velocity (pixels per fixed frame)
        float radius;   // world-space radius (already scaled)
        float scale;    // sprite scale (1.0 or 1.6)
        float mass;     // heavier when bigger (scale^2), used for ball-vs-ball
        float life;     // seconds of life left (starts at BALL_LIFETIME)
        bool  alive;
    };

    // --- Fight helpers ---
    void  fireBallPair();                   // boss auto-fires two balls at the player
    void  updateBalls();                    // one physics step for every ball
    void  stepBall(Ball& b);                // gravity + tile/wall/spike bounce
    void  resolveBallPair(Ball& a, Ball& b);// elastic ball-vs-ball collision
    bool  ballHitsPlayer(const Ball& b) const;
    bool  attackHitsBoss(InputManager* input) const;   // player attack -> boss box
    bool  attackCircleWorld(float& cx, float& cy, float& r) const;  // attack circle in world space
    void  deflectBalls();                              // player attack knocks incoming balls away

    // Circle (cx,cy,r) vs axis-aligned box overlap test.
    static bool circleVsBox(float cx, float cy, float r,
                            float left, float top, float right, float bottom);
    // Does a ball's box sit on a solid tile or outside the arena walls?
    bool  ballBlocked(float cx, float cy, float r) const;

    Background*      background;
    TileMap*         tileMap;
    PlayerAnimation* player;

    // Boss sprite (67Boss.png, 128x128).
    LPDIRECT3DTEXTURE9 bossTex;
    float bossX, bossY;   // world position (top-left)
    float bossW, bossH;   // texture size in pixels

    // Projectile sprite the boss throws (67BossAttack.png, 64x64).
    LPDIRECT3DTEXTURE9 ballTex;
    float ballW, ballH;   // texture size in pixels

    bool  bossAlive;      // false once the player lands the killing hit
    bool  ended;          // true once a WIN / LOSE state has been requested
    float fireTimer;      // seconds until the boss fires the next ball pair
    std::vector<Ball> balls;
};
