#include "BossState.h"
#include "GameEngine.h"
#include "PauseState.h"
#include "EndState.h"
#include "Cheat.h"
#include "GameLog.h"
#include <math.h>
#include <algorithm>

// ---- Tunable fight constants (tuned for a 1/32 s "frame", scaled to real time
//      by `frames = deltaTime * 32` so the feel matches the original prac7). ----
static const float BOSS_FPS          = 32.0f;
static const float BALL_LIFETIME     = 7.0f;   // a ball lives at most 7 seconds
static const float BALL_GRAVITY      = 1.2f;   // pull per frame
static const float BALL_RESTITUTION  = 0.86f;  // bounciness (1 = no energy lost)
static const float BALL_ART_SCALE    = 0.9f;   // draw the 64px ball this big at scale 1
static const float BALL_FIRE_INTERVAL = 7.0f;  // boss fires a new pair every N seconds
static const int   MAX_BALLS         = 8;      // cap live balls so the arena stays fair

BossState::BossState() {
    bossTex = NULL;
    ballTex = NULL;

    // Boss placeholder position (top-left), right side of the arena near the floor.
    bossX = 20.0f * TileMap::TILE;   // col 20
    bossY = 3.0f * TileMap::TILE;    // near the floor
    bossW = 128.0f;                  // real size read in Initialize
    bossH = 128.0f;
    ballW = 64.0f;
    ballH = 64.0f;

    bossAlive = true;
    ended = false;
    fireTimer = 1.0f;   // first volley ~1 second after entering the room
}

void BossState::Initialize(GameEngine* eng) {
    GameState::Initialize(eng);

    background.load(engine->GetGraphics());
    map.load(engine->GetGraphics(), "Assets/Map/Map2.txt");   // boss arena

    // Spawn the warrior at the left entrance; gravity settles them onto the floor.
    player.Initialize(engine->GetGraphics(), D3DXVECTOR2(150.0f, 200.0f));

    bossTex = engine->GetGraphics()->LoadTexture("Assets/Boss/67Boss.png");
    ballTex = engine->GetGraphics()->LoadTexture("Assets/Boss/67BossAttack.png");

    // Read the real texture sizes so the boss hitbox and ball radius stay correct.
    D3DSURFACE_DESC desc;
    if (bossTex && SUCCEEDED(bossTex->GetLevelDesc(0, &desc))) {
        bossW = (float)desc.Width;
        bossH = (float)desc.Height;
    }
    if (ballTex && SUCCEEDED(ballTex->GetLevelDesc(0, &desc))) {
        ballW = (float)desc.Width;
        ballH = (float)desc.Height;
    }

    // Boss room music.
    engine->GetAudio()->Play("BossBGM", AudioManager::BGM);
}

void BossState::Resume() {
    // Back from the pause menu: bring the boss music back.
    engine->GetAudio()->Play("BossBGM", AudioManager::BGM);
}

void BossState::UpdateLogic(Input* input, float deltaTime) {
    // A WIN / LOSE state has already been queued; just wait for the swap.
    if (ended)
        return;

    // Esc opens the pause menu (freezes the boss room).
    if (input->IsKeyJustPressed(DIK_ESCAPE)) {
        GameLog("Player paused the game (Esc)");
        engine->GetStateManager()->PushState(new PauseState());
        return;
    }

    // Backspace returns to the level underneath.
    if (input->IsKeyJustPressed(DIK_BACK)) {
        engine->GetStateManager()->PopState();
        return;
    }

    // Player walks / collides; the boss stays put.
    player.UpdateLogic(input, deltaTime, engine->GetAudio());
    player.ResolveMapCollisions(&map);

    // Camera follows the player, clamped to the left edge.
    D3DXVECTOR2 playerPos = player.GetPosition();
    float camX = playerPos.x - (engine->GetScreenWidth() / 2.0f);
    if (camX < 0.0f) camX = 0.0f;
    engine->GetCamera()->SetPosition(D3DXVECTOR2(camX, 0.0f));
    engine->GetCamera()->Update();

    float hl, ht, hr, hb;
    player.GetWorldHitbox(hl, ht, hr, hb);

    // Player walked into a spike -> LOSE (skipped in cheat / god mode).
    if (!Cheat::enabled() && map.rectSpike(hl, ht, hr, hb)) {
        GameLog("Player hit a spike in the boss room -> Game Over");
        GameLog("DBG: before StopBGM");                            // DEBUG
        engine->GetAudio()->StopBGM();
        GameLog("DBG: after StopBGM, before new EndState");        // DEBUG
        GameState* es = new EndState(EndState::RESULT_LOSE);
        GameLog("DBG: after new EndState, before PushState");      // DEBUG
        engine->GetStateManager()->PushState(es);
        GameLog("DBG: after PushState (queued OK)");               // DEBUG
        ended = true;
        return;
    }

    if (bossAlive) {
        // One clean hit kills the boss -> WIN immediately.
        if (attackHitsBoss()) {
            bossAlive = false;
            GameLog("Player struck the boss -> boss defeated -> Victory");
            GameLog("DBG(win): before StopBGM");                       // DEBUG
            engine->GetAudio()->StopBGM();
            GameLog("DBG(win): after StopBGM, before new EndState");   // DEBUG
            GameState* es = new EndState(EndState::RESULT_WIN);
            GameLog("DBG(win): after new EndState, before PushState"); // DEBUG
            engine->GetStateManager()->PushState(es);
            GameLog("DBG(win): after PushState (queued OK)");          // DEBUG
            ended = true;
            return;
        }

        // Boss stays put and auto-fires two balls at the player on a timer.
        fireTimer -= deltaTime;
        if (fireTimer <= 0.0f) {
            fireBallPair();
            fireTimer = BALL_FIRE_INTERVAL;
        }
    }

    // Advance the balls in flight (real-time, scaled to the 1/32 s tuning).
    float frames = deltaTime * BOSS_FPS;
    updateBalls(frames);

    // Player attack knocks any boss ball in range back out (defend by attacking).
    deflectBalls();

    // A boss ball touched the player -> LOSE (skipped in cheat / god mode).
    if (!Cheat::enabled()) {
        for (size_t i = 0; i < balls.size(); i++) {
            if (balls[i].alive && ballHitsPlayer(balls[i])) {
                GameLog("A boss ball hit the player -> Game Over");
                engine->GetAudio()->StopBGM();
                engine->GetStateManager()->PushState(new EndState(EndState::RESULT_LOSE));
                GameLog("BossState(ball): PushState(EndState) queued");   // DEBUG
                ended = true;
                return;
            }
        }
    }
}

// Attack circle (world space) vs the boss box. Returns false when not attacking.
bool BossState::attackHitsBoss() const {
    D3DXVECTOR2 centre;
    float r;
    if (!player.GetAttackCircleWorld(centre, r))
        return false;
    return circleVsBox(centre.x, centre.y, r, bossX, bossY, bossX + bossW, bossY + bossH);
}

// While the player is attacking, any boss ball caught in the attack circle is
// knocked away instead of hurting the player.
void BossState::deflectBalls() {
    D3DXVECTOR2 centre;
    float r;
    if (!player.GetAttackCircleWorld(centre, r))
        return;   // not attacking -> nothing to deflect

    const float KNOCKBACK_SPEED = 16.0f;   // how hard a deflected ball flies off

    for (size_t i = 0; i < balls.size(); i++) {
        Ball& b = balls[i];
        if (!b.alive)
            continue;

        float dx = b.x - centre.x;
        float dy = b.y - centre.y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist > r + b.radius)
            continue;   // this ball isn't inside the attack circle

        // Direction from the attack centre to the ball (fall back to straight up).
        float nx = (dist > 0.0001f) ? dx / dist : 0.0f;
        float ny = (dist > 0.0001f) ? dy / dist : -1.0f;

        b.vx = nx * KNOCKBACK_SPEED;
        b.vy = ny * KNOCKBACK_SPEED - 6.0f;   // add a little lift so it arcs away
    }
}

// Boss auto-fires two "67" balls at once: one small (scale 1) and one big
// (scale 1.6), both aimed toward the player. Skipped if the arena is full.
void BossState::fireBallPair() {
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
    player.GetWorldHitbox(pl, pt, pr, pb);
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
    engine->GetAudio()->Play("BossAttack");
}

void BossState::updateBalls(float frames) {
    for (size_t i = 0; i < balls.size(); i++) {
        if (!balls[i].alive)
            continue;

        stepBall(balls[i], frames);

        // Count down its 7-second life (frames / 32 = elapsed seconds).
        balls[i].life -= frames / BOSS_FPS;
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
// solid tiles / arena walls, then a spike bounce. `frames` scales to real time.
void BossState::stepBall(Ball& b, float frames) {
    b.vy += BALL_GRAVITY * frames;

    // ---- Horizontal move + bounce ----
    float prevX = b.x;
    b.x += b.vx * frames;
    if (ballBlocked(b.x, b.y, b.radius)) {
        b.x = prevX;
        b.vx = -b.vx * BALL_RESTITUTION;
    }

    // ---- Vertical move + bounce (floor / ceiling / tiles) ----
    float prevY = b.y;
    b.y += b.vy * frames;
    if (ballBlocked(b.x, b.y, b.radius)) {
        b.y = prevY;
        b.vy = -b.vy * BALL_RESTITUTION;
    }

    // ---- Spike bounce (spikes are not "solid", so test them separately) ----
    if (map.rectSpike(b.x - b.radius, b.y - b.radius,
                      b.x + b.radius, b.y + b.radius)) {
        b.y -= 6.0f;
        b.vy = -fabsf(b.vy) * BALL_RESTITUTION - 3.0f;
    }
}

// Elastic collision between two balls of DIFFERENT mass. Separate them, then
// apply the 1D elastic response along the line joining their centres.
void BossState::resolveBallPair(Ball& a, Ball& b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float dist = sqrtf(dx * dx + dy * dy);
    float minDist = a.radius + b.radius;

    if (dist >= minDist || dist <= 0.0001f)
        return;   // not touching (or exactly overlapping)

    float nx = dx / dist;
    float ny = dy / dist;

    // Push them apart so they stop overlapping; the heavier ball gives way less.
    float ma = a.mass, mb = b.mass;
    float overlap = minDist - dist;
    float aShare = mb / (ma + mb);
    float bShare = ma / (ma + mb);
    a.x -= nx * overlap * aShare;  a.y -= ny * overlap * aShare;
    b.x += nx * overlap * bShare;  b.y += ny * overlap * bShare;

    // Velocity along the normal for each ball.
    float an = a.vx * nx + a.vy * ny;
    float bn = b.vx * nx + b.vy * ny;

    if (an - bn <= 0.0f)
        return;   // only resolve if they are approaching

    // 1D elastic collision with restitution, unequal masses.
    float e = BALL_RESTITUTION;
    float sum = ma + mb;
    float anNew = (ma * an + mb * bn + mb * e * (bn - an)) / sum;
    float bnNew = (ma * an + mb * bn + ma * e * (an - bn)) / sum;

    a.vx += (anNew - an) * nx;  a.vy += (anNew - an) * ny;
    b.vx += (bnNew - bn) * nx;  b.vy += (bnNew - bn) * ny;
}

bool BossState::ballHitsPlayer(const Ball& b) const {
    float hl, ht, hr, hb;
    player.GetWorldHitbox(hl, ht, hr, hb);

    // The collider box (32x64 at scale 1) only covers the warrior's torso, but
    // the warrior is DRAWN from a much larger 192x192 sprite cell, so a ball can
    // clearly touch the visible knight (helmet / shield / sword) yet miss the
    // tiny box -> no LOSE. Grow the box into a "hurt box" that matches the drawn
    // character (scaled with item pickups) so a visible hit actually counts.
    float s     = player.GetScale();
    float padX  = 26.0f * s;   // widen out to the shield / sword / body edges
    float padUp = 34.0f * s;   // reach up to the helmet
    hl -= padX;  hr += padX;
    ht -= padUp;               // the box bottom already sits near the feet

    return circleVsBox(b.x, b.y, b.radius, hl, ht, hr, hb);
}

// A ball's box hits a solid tile or leaves the arena (walls, ceiling, bottom).
bool BossState::ballBlocked(float cx, float cy, float r) const {
    float left = cx - r;
    float right = cx + r;
    float top = cy - r;
    float bottom = cy + r;

    const float arenaRight = (float)(TileMap::COLS * TileMap::TILE);
    const float arenaBottom = (float)(TileMap::ROWS * TileMap::TILE);

    if (left < 0.0f || right > arenaRight)  return true;   // side walls
    if (top < 0.0f || bottom > arenaBottom) return true;   // ceiling / bottom

    return map.rectSolid(left, top, right, bottom);
}

bool BossState::circleVsBox(float cx, float cy, float r,
                            float left, float top, float right, float bottom) {
    // Closest point on the box to the circle centre.
    float nx = cx < left ? left : (cx > right ? right : cx);
    float ny = cy < top ? top : (cy > bottom ? bottom : cy);
    float dx = cx - nx;
    float dy = cy - ny;
    return (dx * dx + dy * dy) <= r * r;
}

void BossState::RenderFrame(Graphics* graphics) {
    if (ended) { static bool r = false; if (!r) { GameLog("DBG: Boss RenderFrame START while ended"); r = true; } }   // DEBUG

    Camera* camera = engine->GetCamera();
    float cameraX = camera->GetPosition().x;

    background.render(graphics, camera);
    map.render(graphics, camera, engine->GetScreenWidth());

    // Living boss, scrolled with the room.
    if (bossAlive && bossTex) {
        D3DXMATRIX transM;
        D3DXMatrixTranslation(&transM, bossX - cameraX, bossY, 0.0f);
        graphics->DrawSprite(bossTex, NULL, &transM);
    }

    // Thrown balls (67BossAttack.png, scaled per ball, drawn around the centre).
    if (ballTex) {
        for (size_t i = 0; i < balls.size(); i++) {
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
            graphics->DrawSprite(ballTex, NULL, &world);
        }
    }

    player.RenderFrame(graphics, camera);

    if (ended) { static bool r = false; if (!r) { GameLog("DBG: Boss RenderFrame END while ended"); r = true; } }   // DEBUG
}
