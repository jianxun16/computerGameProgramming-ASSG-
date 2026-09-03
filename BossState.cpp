// author by limzhenxing

#include "BossState.h"
#include "Game.h"
#include "Graphics.h"
#include "InputManager.h"
#include "Background.h"
#include "TileMap.h"
#include "PlayerAnimation.h"
#include "PauseState.h"
#include "EndState.h"
#include "AudioManager.h"
#include "GameLog.h"
#include "Cheat.h"
#include <math.h>
#include <algorithm>

// ---- Tunable fight constants (fixed 32 FPS step, so 1 frame = 1/32 s) ----
static const float FRAME_DT     = 1.0f / 32.0f;  // one update = this many seconds
static const float BALL_LIFETIME = 7.0f;         // a ball lives at most 7 seconds
static const float BALL_GRAVITY  = 1.2f;         // pull per frame (player uses 2.0)
static const float BALL_RESTITUTION = 0.86f;     // bounciness (1 = no energy lost)
static const float BALL_ART_SCALE = 0.9f;        // draw the 64px ball texture this big at scale 1
static const float BALL_FIRE_INTERVAL = 7.0f;    // boss fires a new pair every N seconds
static const int   MAX_BALLS = 8;                // cap live balls so the arena stays fair

BossState::BossState(Game* game) : GameState(game)
{
    background = new Background();
    tileMap = new TileMap();
    player = new PlayerAnimation();
    bossTex = NULL;
    ballTex = NULL;

    // Boss placeholder position (top-left), right side of the arena near the floor.
    bossX = 20.0f * TileMap::TILE;   // col 20
    bossY = 3.0f  * TileMap::TILE;   // near the floor
    bossW = 128.0f;                  // 67Boss.png is 128x128 (real size read in onEnter)
    bossH = 128.0f;
    ballW = 64.0f;                   // 67BossAttack.png is 64x64 (real size read in onEnter)
    ballH = 64.0f;

    bossAlive = true;
    ended     = false;
    fireTimer = 1.0f;   // first volley ~1 second after entering the room
}

BossState::~BossState()
{
    delete background;
    delete tileMap;
    delete player;
    if (bossTex != NULL) { bossTex->Release(); bossTex = NULL; }
    if (ballTex != NULL) { ballTex->Release(); ballTex = NULL; }
}

void BossState::onEnter()
{
    Graphics* g = game->graphics();
    background->load(g->device());
    tileMap->load(g->device(), "Assets/Map/Map2.txt");   // boss arena
    player->load(g->device(), g->width(), g->height());

    D3DXCreateTextureFromFile(g->device(), "Assets/Boss/67Boss.png", &bossTex);
    D3DXCreateTextureFromFile(g->device(), "Assets/Boss/67BossAttack.png", &ballTex);

    // Read the real texture sizes so the boss hitbox and ball radius stay correct.
    D3DSURFACE_DESC desc;
    if (bossTex != NULL && SUCCEEDED(bossTex->GetLevelDesc(0, &desc)))
    {
        bossW = (float)desc.Width;
        bossH = (float)desc.Height;
    }
    if (ballTex != NULL && SUCCEEDED(ballTex->GetLevelDesc(0, &desc)))
    {
        ballW = (float)desc.Width;
        ballH = (float)desc.Height;
    }

    // Spawn on the arena floor at the left entrance.
    player->respawnToStart();
    player->setStartFeet(tileMap->groundTopYAt(player->getFeetWorldX()));
}

void BossState::update(InputManager* input)
{
    // A WIN / LOSE state has already been queued; just wait for the swap.
    if (ended)
        return;

    // Esc opens the pause menu (freezes the boss room).
    if (input->isKeyPressed(DIK_ESCAPE))
    {
        GameLog("Player paused the game (Esc)");
        game->pushState(new PauseState(game));
        return;
    }

    // Backspace returns to the level underneath.
    if (input->isKeyDown(DIK_BACK))
    {
        game->popState();
        return;
    }

    // Player walks / collides; the boss stays put.
    float deltaX = player->update(input, game->audio(), tileMap);
    background->update(deltaX);

    float hl, ht, hr, hb;
    player->getWorldHitbox(hl, ht, hr, hb);

    // Player walked into a spike -> LOSE (skipped in cheat / god mode).
    if (!Cheat::enabled() && tileMap->rectSpike(hl, ht, hr, hb))
    {
        GameLog("Player hit a spike in the boss room -> Game Over");
        game->audio()->stopBGM();
        game->pushState(new EndState(game, EndState::RESULT_LOSE));
        ended = true;
        return;
    }

    if (bossAlive)
    {
        // One clean hit kills the boss -> WIN immediately.
        if (attackHitsBoss(input))
        {
            bossAlive = false;
            GameLog("Player struck the boss -> boss defeated -> Victory");
            game->audio()->stopBGM();
            game->pushState(new EndState(game, EndState::RESULT_WIN));
            ended = true;
            return;
        }

        // Boss stays put and auto-fires two balls at the player on a timer.
        fireTimer -= FRAME_DT;
        if (fireTimer <= 0.0f)
        {
            fireBallPair();
            fireTimer = BALL_FIRE_INTERVAL;
        }
    }

    // Advance the balls in flight: gravity, wall / spike bounces, ball-vs-ball.
    updateBalls();

    // Player attack knocks any boss ball in range back out (defend by attacking).
    deflectBalls();

    // A boss ball touched the player -> LOSE (skipped in cheat / god mode).
    if (!Cheat::enabled())
    {
        for (size_t i = 0; i < balls.size(); i++)
        {
            if (balls[i].alive && ballHitsPlayer(balls[i]))
            {
                GameLog("A boss ball hit the player -> Game Over");
                game->audio()->stopBGM();
                game->pushState(new EndState(game, EndState::RESULT_LOSE));
                ended = true;
                return;
            }
        }
    }
}

// The player's attack circle is in SCREEN space; add cameraX to convert its
// centre to WORLD space. Returns false when the player isn't attacking.
bool BossState::attackCircleWorld(float& cx, float& cy, float& r) const
{
    D3DXVECTOR3 centre;
    if (!player->getAttackCircle(centre, r))
        return false;   // not attacking this frame

    cx = centre.x + player->getCameraX();
    cy = centre.y;   // vertical never scrolls
    return true;
}

// Attack circle (world space) vs the boss box.
bool BossState::attackHitsBoss(InputManager* input) const
{
    float cx, cy, r;
    if (!attackCircleWorld(cx, cy, r))
        return false;
    return circleVsBox(cx, cy, r, bossX, bossY, bossX + bossW, bossY + bossH);
}

// While the player is attacking, any boss ball caught in the attack circle is
// knocked away (fired back out along the line from the attack centre to the
// ball, with a small upward arc) instead of hurting the player.
void BossState::deflectBalls()
{
    float cx, cy, r;
    if (!attackCircleWorld(cx, cy, r))
        return;   // not attacking -> nothing to deflect

    const float KNOCKBACK_SPEED = 16.0f;   // how hard a deflected ball flies off

    for (size_t i = 0; i < balls.size(); i++)
    {
        Ball& b = balls[i];
        if (!b.alive)
            continue;

        float dx = b.x - cx;
        float dy = b.y - cy;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist > r + b.radius)
            continue;   // this ball isn't inside the attack circle

        // Direction from the attack centre to the ball (fall back to straight
        // up if they're exactly on top of each other).
        float nx = (dist > 0.0001f) ? dx / dist : 0.0f;
        float ny = (dist > 0.0001f) ? dy / dist : -1.0f;

        b.vx = nx * KNOCKBACK_SPEED;
        b.vy = ny * KNOCKBACK_SPEED - 6.0f;   // add a little lift so it arcs away
    }
}

// Boss auto-fires two "67" balls at once from its own position: one small
// (scale 1) and one big (scale 1.6), both aimed toward the player so they fly
// over and can hit. Skipped if the arena is already full of balls.
void BossState::fireBallPair()
{
    int aliveCount = 0;
    for (size_t i = 0; i < balls.size(); i++)
        if (balls[i].alive) aliveCount++;
    if (aliveCount >= MAX_BALLS)
        return;

    float cx = bossX + bossW * 0.5f;
    float cy = bossY + bossH * 0.5f;

    // radius = half of the drawn sprite (ballW * BALL_ART_SCALE * scale).
    float baseR = ballW * 0.5f * BALL_ART_SCALE;

    // Aim horizontally toward the player (dir = -1 left, +1 right).
    float pl, pt, pr, pb;
    player->getWorldHitbox(pl, pt, pr, pb);
    float playerCX = (pl + pr) * 0.5f;
    float dir = (playerCX < cx) ? -1.0f : 1.0f;

    Ball ballSmall;
    ballSmall.x = cx;  ballSmall.y = cy;
    ballSmall.vx = dir * 8.0f;  ballSmall.vy = -14.0f;   // flat, fast arc
    ballSmall.scale = 1.0f;
    ballSmall.radius = baseR * ballSmall.scale;
    ballSmall.mass = ballSmall.scale * ballSmall.scale;  // area-like mass
    ballSmall.life = BALL_LIFETIME;
    ballSmall.alive = true;
    balls.push_back(ballSmall);

    Ball ballBig;
    ballBig.x = cx;  ballBig.y = cy;
    ballBig.vx = dir * 5.0f;  ballBig.vy = -21.0f;       // higher, slower lob
    ballBig.scale = 1.6f;
    ballBig.radius = baseR * ballBig.scale;
    ballBig.mass = ballBig.scale * ballBig.scale;        // ~2.56x the small ball
    ballBig.life = BALL_LIFETIME;
    ballBig.alive = true;
    balls.push_back(ballBig);

    // Boss throw sound effect.
    game->audio()->playBossAttack();
}

void BossState::updateBalls()
{
    for (size_t i = 0; i < balls.size(); i++)
    {
        if (!balls[i].alive)
            continue;

        stepBall(balls[i]);

        // Count down its 7-second life.
        balls[i].life -= FRAME_DT;
        if (balls[i].life <= 0.0f)
            balls[i].alive = false;
    }

    // Ball-vs-ball physics (only a handful of balls, so test every pair).
    for (size_t i = 0; i < balls.size(); i++)
        for (size_t j = i + 1; j < balls.size(); j++)
            if (balls[i].alive && balls[j].alive)
                resolveBallPair(balls[i], balls[j]);

    // Drop expired balls so the list can't grow forever over a long fight.
    balls.erase(std::remove_if(balls.begin(), balls.end(),
                               [](const Ball& b) { return !b.alive; }),
                balls.end());
}

// One physics step for a single ball: gravity, then axis-separated bounces off
// solid tiles / arena walls, then a spike bounce.
void BossState::stepBall(Ball& b)
{
    b.vy += BALL_GRAVITY;

    // ---- Horizontal move + bounce ----
    float prevX = b.x;
    b.x += b.vx;
    if (ballBlocked(b.x, b.y, b.radius))
    {
        b.x = prevX;
        b.vx = -b.vx * BALL_RESTITUTION;
    }

    // ---- Vertical move + bounce (floor / ceiling / tiles) ----
    float prevY = b.y;
    b.y += b.vy;
    if (ballBlocked(b.x, b.y, b.radius))
    {
        b.y = prevY;
        b.vy = -b.vy * BALL_RESTITUTION;
    }

    // ---- Spike bounce (spikes are not "solid", so test them separately) ----
    if (tileMap->rectSpike(b.x - b.radius, b.y - b.radius,
                           b.x + b.radius, b.y + b.radius))
    {
        // Eject upward out of the spike and reflect the vertical velocity.
        b.y -= 6.0f;
        b.vy = -fabsf(b.vy) * BALL_RESTITUTION - 3.0f;
    }
}

// Elastic collision between two balls of DIFFERENT mass (the bigger ball is
// heavier, so it barely moves while the small one gets flung). Separate them,
// then apply the 1D elastic response along the line joining their centres.
void BossState::resolveBallPair(Ball& a, Ball& b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float dist = sqrtf(dx * dx + dy * dy);
    float minDist = a.radius + b.radius;

    if (dist >= minDist || dist <= 0.0001f)
        return;   // not touching (or exactly overlapping) -> nothing to do

    // Collision normal (unit vector from a to b).
    float nx = dx / dist;
    float ny = dy / dist;

    // Push them apart so they stop overlapping. The heavier ball gives way less:
    // each is moved by the OTHER ball's share of the mass.
    float ma = a.mass, mb = b.mass;
    float overlap = minDist - dist;
    float aShare = mb / (ma + mb);   // small ball (light) -> big share -> moves more
    float bShare = ma / (ma + mb);
    a.x -= nx * overlap * aShare;  a.y -= ny * overlap * aShare;
    b.x += nx * overlap * bShare;  b.y += ny * overlap * bShare;

    // Velocity along the normal for each ball.
    float an = a.vx * nx + a.vy * ny;
    float bn = b.vx * nx + b.vy * ny;

    // Only resolve if they are approaching (normal speeds closing).
    if (an - bn <= 0.0f)
        return;

    // 1D elastic collision with restitution, unequal masses. New normal speeds:
    //   an' = (ma*an + mb*bn + mb*e*(bn - an)) / (ma + mb)
    //   bn' = (ma*an + mb*bn + ma*e*(an - bn)) / (ma + mb)
    float e = BALL_RESTITUTION;
    float sum = ma + mb;
    float anNew = (ma * an + mb * bn + mb * e * (bn - an)) / sum;
    float bnNew = (ma * an + mb * bn + ma * e * (an - bn)) / sum;

    a.vx += (anNew - an) * nx;  a.vy += (anNew - an) * ny;
    b.vx += (bnNew - bn) * nx;  b.vy += (bnNew - bn) * ny;
}

bool BossState::ballHitsPlayer(const Ball& b) const
{
    float hl, ht, hr, hb;
    player->getWorldHitbox(hl, ht, hr, hb);
    return circleVsBox(b.x, b.y, b.radius, hl, ht, hr, hb);
}

// A ball's bounding box hits a solid tile or leaves the arena (left/right walls,
// ceiling, and the very bottom safety floor).
bool BossState::ballBlocked(float cx, float cy, float r) const
{
    float left   = cx - r;
    float right  = cx + r;
    float top    = cy - r;
    float bottom = cy + r;

    const float arenaRight  = (float)(TileMap::COLS * TileMap::TILE);
    const float arenaBottom = (float)(TileMap::ROWS * TileMap::TILE);

    if (left < 0.0f || right > arenaRight)  return true;   // side walls
    if (top < 0.0f  || bottom > arenaBottom) return true;  // ceiling / bottom

    return tileMap->rectSolid(left, top, right, bottom);
}

bool BossState::circleVsBox(float cx, float cy, float r,
                            float left, float top, float right, float bottom)
{
    // Closest point on the box to the circle centre.
    float nx = cx < left ? left : (cx > right ? right : cx);
    float ny = cy < top  ? top  : (cy > bottom ? bottom : cy);
    float dx = cx - nx;
    float dy = cy - ny;
    return (dx * dx + dy * dy) <= r * r;
}

void BossState::render(Graphics* gfx)
{
    float cameraX = player->getCameraX();

    background->render(gfx->sprite(), gfx->width());
    tileMap->render(gfx->sprite(), cameraX, gfx->width());

    LPD3DXSPRITE sprite = gfx->sprite();

    // Living boss, scrolled with the room.
    if (bossAlive && bossTex != NULL)
    {
        D3DXVECTOR3 pos(bossX - cameraX, bossY, 0.0f);
        sprite->Draw(bossTex, NULL, NULL, &pos, D3DCOLOR_XRGB(255, 255, 255));
    }

    // Thrown balls (67BossAttack.png, scaled per ball, drawn around the centre).
    if (ballTex != NULL)
    {
        D3DXMATRIX identity;
        D3DXMatrixIdentity(&identity);

        for (size_t i = 0; i < balls.size(); i++)
        {
            if (!balls[i].alive)
                continue;

            float artScale = BALL_ART_SCALE * balls[i].scale;   // texture zoom
            float half = balls[i].radius;                       // = ballW/2 * artScale

            D3DXMATRIX scaleM, transM, world;
            D3DXMatrixScaling(&scaleM, artScale, artScale, 1.0f);
            D3DXMatrixTranslation(&transM,
                                  balls[i].x - half - cameraX,
                                  balls[i].y - half, 0.0f);
            world = scaleM * transM;
            sprite->SetTransform(&world);

            D3DXVECTOR3 zero(0.0f, 0.0f, 0.0f);
            sprite->Draw(ballTex, NULL, NULL, &zero, D3DCOLOR_XRGB(255, 255, 255));
        }

        sprite->SetTransform(&identity);
    }

    player->render(sprite);
}
