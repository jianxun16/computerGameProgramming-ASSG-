#include "PlayerAnimation.h"
#include "AudioManager.h"
#include "InputManager.h"
#include "TileMap.h"
#include "GameLog.h"
#include <Windows.h>
#include <math.h>
#include <iostream>

using namespace std;

PlayerAnimation::PlayerAnimation()
{
    sheet = NULL;
    rangeTexture = NULL;

    // Frame blocks inside Warrior_Anim.png (5-column grid).
    //idle
    idleAnim.startCol   = 0; 
    idleAnim.startRow   = 0; 
    idleAnim.cols = 3; 
    idleAnim.count = 8;

    //attack
    attackAnim.startCol = 3; 
    attackAnim.startRow = 0; 
    attackAnim.cols = 2; 
    attackAnim.count = 4;

    //run
    runAnim.startCol    = 0;
    runAnim.startRow    = 3;
    runAnim.cols = 3;
    runAnim.count = 6;

    // Physics. Gravity + tile collision now live in the PlayerCollision class.
    velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    groundY = 0.0f;
    spriteMovementSpeed = 5;    // run speed

    playerWorldPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

    // Animation / state.
    state = STATE_IDLE;
    lastLoggedState = STATE_IDLE;
    currentFrame = 0;
    prevAnimId = 0;
    animationTimer = 0.0f;
    animationSpeed = 0.1f;
    attacking = false;
    lastFacingLeft = false;   // start facing right
    isJumping = false;

    // Uniform zoom. Change this (or call setScale) to resize the whole
    // character safely - feet stay planted, attack circle stays proportional.
    scale = 1.0f;

    // Attack range (UNSCALED; multiplied by scale when used).
    attackRangeRadius = 55.0f;
    attackRangeOffset = 60.0f;
    attackRangeUp     = 45.0f;
}

PlayerAnimation::~PlayerAnimation()
{
    release();
}

// Build a soft translucent ring so the attack range is visible on screen.
static LPDIRECT3DTEXTURE9 MakeRingTexture(IDirect3DDevice9* device)
{
    const int SZ = 128;
    LPDIRECT3DTEXTURE9 tex = NULL;
    if (FAILED(device->CreateTexture(SZ, SZ, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex, NULL)))
        return NULL;

    D3DLOCKED_RECT lr;
    tex->LockRect(0, &lr, NULL, 0);
    unsigned char* base = (unsigned char*)lr.pBits;

    float c = (SZ - 1) / 2.0f;
    float outer = c;            // outline radius
    for (int y = 0; y < SZ; y++)
    {
        DWORD* row = (DWORD*)(base + y * lr.Pitch);
        for (int x = 0; x < SZ; x++)
        {
            float dx = x - c, dy = y - c;
            float d = sqrtf(dx * dx + dy * dy);

            DWORD a = 0;
            if (d <= outer)
            {
                // translucent red fill, stronger ring near the edge
                a = 40;
                if (d >= outer - 4.0f) a = 200;
            }
            row[x] = (a << 24) | (220 << 16) | (40 << 8) | 40;   // ARGB red, show the attack range
        }
    }
    tex->UnlockRect(0);
    return tex;
}

bool PlayerAnimation::load(IDirect3DDevice9* device, int screenWidth, int screenHeight)
{
    HRESULT hr = D3DXCreateTextureFromFile(device, "Assets/Player/Warrior_Anim.png", &sheet);
    if (FAILED(hr))
        MessageBox(NULL, "Warrior_Anim.png not found!", "Error", MB_OK);

    rangeTexture = MakeRingTexture(device);

    // Place the frame so the character's feet (at FEET_Y inside the cell) rest
    // on the ground line near the bottom of the window.
    const float groundLineY = (float)screenHeight - 40.0f;
    groundY = groundLineY - FEET_Y;

    // Keep the character horizontally centred; the world scrolls around it.
    spritePosition.x = (screenWidth - FRAME_SIZE) / 2.0f;
    spritePosition.y = groundY;
    spritePosition.z = 0;

    return true;
}

const PlayerAnimation::Anim& PlayerAnimation::currentAnim() const
{
    if (state == STATE_ATTACK) return attackAnim;
    if (state == STATE_RUN_LEFT || state == STATE_RUN_RIGHT) return runAnim;
    return idleAnim;
}

int PlayerAnimation::animId() const
{
    if (state == STATE_ATTACK) return 2;
    if (state == STATE_RUN_LEFT || state == STATE_RUN_RIGHT) return 1;
    return 0;
}

bool PlayerAnimation::facingLeft() const
{
    if (state == STATE_RUN_LEFT)  return true;
    if (state == STATE_RUN_RIGHT) return false;
    return lastFacingLeft;   // idle / attack keep the last direction
}

float PlayerAnimation::update(InputManager* input, AudioManager* audioManager, TileMap* map)
{
    // ----- Read input -----
    bool left  = input->isKeyDown(DIK_A);
    bool right = input->isKeyDown(DIK_D);

    // Left mouse click (edge-detected in InputManager) -> one click = one attack.
    bool attackClicked = input->mouseLeftClicked();

    // ----- Horizontal movement + facing -----
    if (left)
    {
        velocity.x = (float)-spriteMovementSpeed;
        lastFacingLeft = true;
    }
    else if (right)
    {
        velocity.x = (float)spriteMovementSpeed;
        lastFacingLeft = false;
    }
    else
    {
        velocity.x = 0;
    }

    // ----- Jump -----
    if (input->isKeyDown(DIK_SPACE) && !isJumping)
    {
        audioManager->playJump();        // PlayerJump.wav
        velocity.y = -26.0f;             // jump power (higher = jump higher)
        isJumping = true;
        GameLog("Player jumped (Space)");
    }

    // ----- Start an attack -----
    // Any left click (anywhere on screen), at any time, restarts the attack
    // animation from frame 0. (Knockback is handled by a teammate's module.)
    if (attackClicked)
    {
        attacking = true;
        currentFrame = 0;
        animationTimer = 0.0f;
        audioManager->playSlash();       // SwordSlash.wav
        GameLog("Player attacked (Left click)");
    }

    // ----- Choose the state -----
    if (attacking)
        state = STATE_ATTACK;
    else if (left)
        state = STATE_RUN_LEFT;
    else if (right)
        state = STATE_RUN_RIGHT;
    else
        state = STATE_IDLE;

    // Log movement to the CLI only when it CHANGES (so holding a key doesn't
    // spam a line every frame). Attack is logged on click above, not here.
    if (state != lastLoggedState)
    {
        if (state == STATE_RUN_RIGHT)     GameLog("Player is walking RIGHT (D)");
        else if (state == STATE_RUN_LEFT) GameLog("Player is walking LEFT (A)");
        else if (state == STATE_IDLE)     GameLog("Player stopped (idle)");
        lastLoggedState = state;
    }

    // Reset the frame when the ANIMATION changes (idle<->run<->attack), but not
    // when only the run direction flips (same run block).
    if (animId() != prevAnimId)
    {
        currentFrame = 0;
        animationTimer = 0.0f;
        prevAnimId = animId();
    }

    // ----- Advance the animation -----
    animationTimer += 0.02f;
    if (animationTimer >= animationSpeed)
    {
        animationTimer = 0.0f;
        currentFrame++;

        if (currentFrame >= currentAnim().count)
        {
            if (state == STATE_ATTACK)
                attacking = false;   // attack finished -> idle/run next update
            currentFrame = 0;
        }
    }




    // ----- Apply movement with tile collision -----
    // The physics/collision now lives in PlayerCollision; hand it our position,
    // velocity and jump flag to resolve in place.
    collision.resolve(map, (float)FEET_CENTER_X, (float)FEET_Y, scale,
                      spritePosition, playerWorldPosition, velocity,
                      isJumping, groundY);

    return velocity.x;   // drives the parallax background + tile map scroll
}

void PlayerAnimation::respawnToStart()
{
    playerWorldPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    isJumping = false;
    scale = 2.0f;                 // lose any grow/shrink from items
    state = STATE_IDLE;
    currentFrame = 0;
    lastFacingLeft = false;
}

void PlayerAnimation::getWorldHitbox(float& left, float& top, float& right, float& bottom) const
{
    float feetWX = spritePosition.x + FEET_CENTER_X + playerWorldPosition.x;
    float feetY  = spritePosition.y + FEET_Y;
    float halfW  = collision.getHalfWidth(scale);   // same box the collision uses
    float bodyH  = collision.getBodyHeight(scale);

    left   = feetWX - halfW;
    right  = feetWX + halfW;
    bottom = feetY;
    top    = feetY - bodyH;
}

bool PlayerAnimation::getAttackCircle(D3DXVECTOR3& centre, float& radius) const
{
    if (!attacking)
        return false;

    // Measure everything from the feet anchor and scale by `scale`, so the
    // circle grows with the character and keeps the same relative position.
    float dir = facingLeft() ? -1.0f : 1.0f;
    float anchorX = spritePosition.x + FEET_CENTER_X;
    float anchorY = spritePosition.y + FEET_Y;

    centre.x = anchorX + dir * attackRangeOffset * scale;
    centre.y = anchorY - attackRangeUp * scale;     // roughly body height
    centre.z = 0.0f;
    radius = attackRangeRadius * scale;
    return true;
}

void PlayerAnimation::render(LPD3DXSPRITE spriteBrush)
{
    const Anim& anim = currentAnim();

    // Frame -> cell inside the block (left->right, top->bottom).
    int localCol = currentFrame % anim.cols;
    int localRow = currentFrame / anim.cols;
    int col = anim.startCol + localCol;
    int row = anim.startRow + localRow;

    RECT src;
    src.left   = col * FRAME_SIZE;
    src.top    = row * FRAME_SIZE;
    src.right  = src.left + FRAME_SIZE;
    src.bottom = src.top + FRAME_SIZE;

    // Draw the character, scaled/flipped around the feet anchor so he stays on
    // the same spot on the ground no matter what `scale` is.
    const float anchorX = spritePosition.x + FEET_CENTER_X;
    const float anchorY = spritePosition.y + FEET_Y;
    float scaleX = facingLeft() ? -scale : scale;   // negative = mirrored

    D3DXMATRIX toOrigin, scl, back, world, identity;
    D3DXMatrixTranslation(&toOrigin, -anchorX, -anchorY, 0.0f);
    D3DXMatrixScaling(&scl, scaleX, scale, 1.0f);
    D3DXMatrixTranslation(&back, anchorX, anchorY, 0.0f);
    world = toOrigin * scl * back;
    spriteBrush->SetTransform(&world);

    spriteBrush->Draw(sheet, &src, NULL, &spritePosition, D3DCOLOR_XRGB(255, 255, 255));

    D3DXMatrixIdentity(&identity);
    spriteBrush->SetTransform(&identity);

    // ----- Attack range circle -----
    D3DXVECTOR3 centre;
    float radius;
    if (rangeTexture && getAttackCircle(centre, radius))
    {
        float s = (2.0f * radius) / 128.0f;   // ring texture is 128x128
        D3DXMATRIX rs, rt, rw;
        D3DXMatrixScaling(&rs, s, s, 1.0f);
        D3DXMatrixTranslation(&rt, centre.x - radius, centre.y - radius, 0.0f);
        rw = rs * rt;
        spriteBrush->SetTransform(&rw);

        D3DXVECTOR3 zero(0.0f, 0.0f, 0.0f);
        spriteBrush->Draw(rangeTexture, NULL, NULL, &zero, D3DCOLOR_ARGB(255, 255, 255, 255));

        spriteBrush->SetTransform(&identity);
    }
}

void PlayerAnimation::release()
{
    if (sheet != NULL)        { sheet->Release();        sheet = NULL; }
    if (rangeTexture != NULL) { rangeTexture->Release(); rangeTexture = NULL; }
}
