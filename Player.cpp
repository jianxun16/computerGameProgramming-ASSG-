#include "Player.h"

Player::Player() {
    spriteSheet = nullptr;
    moveSpeed = 300.0f; // Pixels per second
    jumpForce = -400.0f;
    isJumping = false;
    facingLeft = false;
    isAttacking = false;
    attackTimer = 0.0f;
    charScale = 1.0f;
    currentState = IDLE;

    // Offset the sprite so the bottom-center of the character visually 
    // aligns with the mathematical center of the Physics box
    spriteOffset = D3DXVECTOR2(-100.0f, -105.0f); // Derived from old FEET_CENTER_X and FEET_Y

    idleAnim = { 0, 0, 3, 8 };
    attackAnim = { 3, 0, 2, 4 };
    runAnim = { 0, 3, 3, 6 };
}

Player::~Player() {
    // Let the AssetManager or Graphics wrapper handle texture cleanup to prevent double-deletes
}

void Player::Initialize(Graphics* graphics, D3DXVECTOR2 startPos) {
    // 1. Setup Physics Object Base
    SetPosition(startPos);
    InitializePhysics(1.0f, 0.0f, 0.0f, 9.8f); // Mass, Bounce, Drag, Gravity Scale
    charScale = 1.0f;
    SetColliderBox((float)BASE_BOX_W, (float)BASE_BOX_H); // Width and Height of the hit box

    // 2. Load Texture
    spriteSheet = graphics->LoadTexture("Assets/Player/Warrior_Anim.png");

    // 3. Setup Animation Controller
    // 1024x1024 sheet, 5 cols, 5 rows, 8 max frames, 0.1f speed, Left to Right
    anim.Initialize(1024, 1024, 5, 5, 8, 0.1f, CycleDirection::LeftToRight);
    ChangeState(IDLE);
}

void Player::ChangeState(AnimState newState) {
    if (currentState == newState) return;

    currentState = newState;
    anim.SetTrack((int)newState);
    anim.Reset();   // snap to frame 0 so the new animation starts clean
}

void Player::UpdateLogic(Input* input, float deltaTime, AudioManager* audio) {
    D3DXVECTOR2 vel = GetVelocity();

    // 1. Horizontal Input
    // Platformers usually override X velocity directly for crisp, responsive controls 
    // rather than using gradual force/friction.
    vel.x = 0.0f;
    if (input->IsKeyDown(DIK_A)) {
        vel.x = -moveSpeed;
        facingLeft = true;
    }
    else if (input->IsKeyDown(DIK_D)) {
        vel.x = moveSpeed;
        facingLeft = false;
    }

    // 2. Jump Input
    if (input->IsKeyJustPressed(DIK_SPACE) && !isJumping) {
        vel.y = jumpForce; // Direct velocity impulse for the jump
        isJumping = true;
        if (audio) audio->PlayPitchSFX("JumpSFX",2);
    }

    // Save the snappy input velocity back to the physics base before integration
    SetVelocity(vel);

    // 3. Apply Gravity
    // Pass a pure downward vector. The PhysicsObject calculates the mass and delta time.
    ApplyGravity(D3DXVECTOR2(0.0f, 100.0f));

    // 4. Attack Input
    if (input->IsMouseButtonJustPressed(0) && !isAttacking) {
        isAttacking = true;
        attackTimer = 0.30f;
        if (audio) audio->Play("SlashSFX");
    }

    if (isAttacking) {
        attackTimer -= deltaTime;
        if (attackTimer <= 0.0f) {
            isAttacking = false;
            attackTimer = 0.0f;
        }
    }

    // 5. Determine Animation State
    if (isAttacking) ChangeState(ATTACK);
    else if (vel.x != 0.0f) ChangeState(RUN);
    else ChangeState(IDLE);

    // 6. Let the OOP Framework handle the movement!
    PhysicsObject::UpdateLogic(deltaTime);

    // 7. Tick animations
    anim.Update(deltaTime);
}

void Player::ResolveMapCollisions(TileMap* map) {
    D3DXVECTOR2 pos = GetPosition();
    D3DXVECTOR2 vel = GetVelocity();

    // Failsafe Floor (Prevents infinite falling if map fails to load)
    if (pos.y > 600.0f) {
        pos.y = 600.0f;
        vel.y = 0.0f;
        isJumping = false;
    }

    if (!map) {
        SetPosition(pos);
        SetVelocity(vel);
        return;
    }

    float halfW = GetBoxWidth() / 2.0f;
    float halfH = GetBoxHeight() / 2.0f;

    // Floor Collision (Falling down)
    if (vel.y > 0) {
        // Shaving 4 pixels off the sides prevents getting stuck on wall seams
        if (map->rectSolid(pos.x - halfW + 4.0f, pos.y + halfH - 4.0f, pos.x + halfW - 4.0f, pos.y + halfH)) {
            int tileRow = (int)floorf((pos.y + halfH) / TileMap::TILE);
            pos.y = (tileRow * (float)TileMap::TILE) - halfH; // Instantly push out of the floor
            vel.y = 0;
            isJumping = false;
        }
    }
    // Ceiling Collision (Jumping up)
    else if (vel.y < 0) {
        if (map->rectSolid(pos.x - halfW + 4.0f, pos.y - halfH - 4.0f, pos.x + halfW - 4.0f, pos.y - halfH + 4.0f)) {
            int tileRow = (int)floorf((pos.y - halfH) / TileMap::TILE);
            pos.y = ((tileRow + 1) * (float)TileMap::TILE) + halfH; // Eject downwards
            vel.y = 0;
        }
    }

    // Right Wall Collision
    if (vel.x > 0) {
        if (map->rectSolid(pos.x + halfW - 4.0f, pos.y - halfH + 4.0f, pos.x + halfW, pos.y + halfH - 4.0f)) {
            int tileCol = (int)floorf((pos.x + halfW) / TileMap::TILE);
            pos.x = (tileCol * (float)TileMap::TILE) - halfW; // Eject left
            vel.x = 0;
        }
    }
    // Left Wall Collision
    else if (vel.x < 0) {
        if (map->rectSolid(pos.x - halfW, pos.y - halfH + 4.0f, pos.x - halfW + 4.0f, pos.y + halfH - 4.0f)) {
            int tileCol = (int)floorf((pos.x - halfW) / TileMap::TILE);
            pos.x = ((tileCol + 1) * (float)TileMap::TILE) + halfW; // Eject right
            vel.x = 0;
        }
    }

    SetPosition(pos);
    SetVelocity(vel);
}
void Player::RenderFrame(Graphics* graphics, Camera* camera) {
    if (!spriteSheet) return;

    // 1. CUSTOM BLOCK ANIMATION SLICING
    // Grab the correct block bounds based on your current state
    int frame = anim.GetCurrentFrame();
    AnimBlock block = idleAnim;
    if (currentState == ATTACK) block = attackAnim;
    else if (currentState == RUN) block = runAnim;

    // Math to wrap frames across rows inside the specific block
    frame = frame % block.count;
    int localCol = frame % block.cols;
    int localRow = frame / block.cols;

    int finalCol = block.startCol + localCol;
    int finalRow = block.startRow + localRow;

    // Set the 192x192 cell
    RECT srcRect = { finalCol * 192, finalRow * 192, (finalCol + 1) * 192, (finalRow + 1) * 192 };

    // 2. CALCULATE SCREEN POSITION
    D3DXVECTOR2 screenPos = GetPosition();
    if (camera) {
        screenPos.x -= camera->GetPosition().x;
        screenPos.y -= camera->GetPosition().y;
    }

    // 3. MATRIX MATH (Fixes the backwards teleport bug)
    D3DXMATRIX toOrigin, scaleMat, toScreen, finalMat;

    // Move origin to the visual center/feet of the 192x192 cell (-96, -137)
    D3DXMatrixTranslation(&toOrigin, spriteOffset.x, spriteOffset.y, 0.0f);

    // Scale and flip horizontally around that new center origin
    float scaleX = (facingLeft ? -1.0f : 1.0f) * charScale;
    D3DXMatrixScaling(&scaleMat, scaleX, charScale, 1.0f);

    // Translate the anchored, scaled sprite to the actual screen coordinates
    D3DXMatrixTranslation(&toScreen, screenPos.x, screenPos.y, 0.0f);

    finalMat = toOrigin * scaleMat * toScreen;

    // 4. DRAW
    graphics->DrawSprite(spriteSheet, &srcRect, &finalMat);
}

void Player::GetWorldHitbox(float& left, float& top, float& right, float& bottom) const {
    D3DXVECTOR2 pos = GetPosition();
    float halfW = GetBoxWidth() / 2.0f;
    float halfH = GetBoxHeight() / 2.0f;
    left = pos.x - halfW;
    top = pos.y - halfH;
    right = pos.x + halfW;
    bottom = pos.y + halfH;
}

bool Player::GetAttackCircleWorld(D3DXVECTOR2& centre, float& radius) const {
    if (!isAttacking) return false;   // circle only exists during the swing

    float half = GetBoxWidth() / 2.0f;
    radius = 60.0f * charScale;

    // Sit the circle just in front of the character, at chest/centre height.
    float dir = facingLeft ? -1.0f : 1.0f;
    centre.x = GetPosition().x + dir * (half + radius * 0.5f);
    centre.y = GetPosition().y;
    return true;
}

void Player::SetScale(float s) {
    if (s < 1.0f) s = 1.0f;   // never smaller than the original size
    if (s > 4.0f) s = 4.0f;   // clamp the maximum

    // Keep the feet planted while the box resizes around them.
    float feetY = GetPosition().y + GetBoxHeight() / 2.0f;
    charScale = s;
    SetColliderBox(BASE_BOX_W * s, BASE_BOX_H * s);

    D3DXVECTOR2 p = GetPosition();
    p.y = feetY - GetBoxHeight() / 2.0f;
    SetPosition(p);
}