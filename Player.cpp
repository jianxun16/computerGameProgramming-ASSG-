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
    spriteOffset = D3DXVECTOR2(-96.0f, -137.0f); // Derived from old FEET_CENTER_X and FEET_Y
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
    // anim.Reset(); // TODO: Add a Reset() function to your AnimationController to snap to frame 0
}

void Player::UpdateLogic(Input* input, float deltaTime, AudioManager* audio) {
    // 1. Get current velocity from the physics base
    D3DXVECTOR2 vel = GetVelocity();

    // 2. Horizontal Input
    vel.x = 0.0f; // Reset horizontal every frame (no ice-skating)
    if (input->IsKeyDown(DIK_A)) {
        vel.x = -moveSpeed;
        facingLeft = true;
    }
    else if (input->IsKeyDown(DIK_D)) {
        vel.x = moveSpeed;
        facingLeft = false;
    }

    // 3. Jump Input
    if (input->IsKeyJustPressed(DIK_SPACE) && !isJumping) {
        vel.y = jumpForce;
        isJumping = true;
        if (audio) audio->Play("JumpSFX");
    }

    // 4. Attack Input (one-shot: fires an attack that lasts attackTimer seconds)
    if (input->IsMouseButtonJustPressed(0) && !isAttacking) {
        isAttacking = true;
        attackTimer = 0.30f;   // attack (and its hit circle) stays active this long
        if (audio) audio->Play("SlashSFX");
    }

    // Tick the one-shot attack down; clear it when the swing finishes.
    if (isAttacking) {
        attackTimer -= deltaTime;
        if (attackTimer <= 0.0f) {
            isAttacking = false;
            attackTimer = 0.0f;
        }
    }

    // 5. Determine Animation State
    if (isAttacking) {
        ChangeState(ATTACK);
    }
    else if (vel.x != 0.0f) {
        ChangeState(RUN);
    }
    else {
        ChangeState(IDLE);
    }

    // 6. Apply Gravity and Update Base Object
    ApplyGravity(D3DXVECTOR2(0, 980.0f * deltaTime)); // 9.8 scaled for pixels
    SetVelocity(vel);

    // PhysicsObject::UpdateLogic will apply the velocity to the Position
    PhysicsObject::UpdateLogic(deltaTime);

    // 7. Tick the animation clock
    anim.Update(deltaTime);
}

void Player::ResolveMapCollisions(TileMap* map) {
    if (!map) return;

    D3DXVECTOR2 pos = GetPosition();
    D3DXVECTOR2 vel = GetVelocity();

    float halfW = GetBoxWidth() / 2.0f;
    float halfH = GetBoxHeight() / 2.0f;

    // Check floor collision (falling down)
    if (vel.y > 0) {
        // We check a small box right under the player's feet
        if (map->rectSolid(pos.x - halfW, pos.y + halfH, pos.x + halfW, pos.y + halfH + 4.0f)) {
            // Snap to the top of the tile grid (Assuming 64x64 tiles)
            int tileRow = (int)((pos.y + halfH) / 64);
            pos.y = (tileRow * 64) - halfH;

            vel.y = 0;
            isJumping = false;
        }
    }

    // Check head collision (jumping up into a ceiling)
    if (vel.y < 0) {
        if (map->rectSolid(pos.x - halfW, pos.y - halfH - 4.0f, pos.x + halfW, pos.y - halfH)) {
            vel.y = 0; // Stop upward momentum
        }
    }

    // Check wall collisions (moving left/right)
    // Note: A robust engine checks X and Y axis resolutions separately to slide along walls.
    if (vel.x > 0) {
        if (map->rectSolid(pos.x + halfW, pos.y - halfH + 4.0f, pos.x + halfW + 4.0f, pos.y + halfH - 4.0f)) {
            vel.x = 0;
        }
    }
    else if (vel.x < 0) {
        if (map->rectSolid(pos.x - halfW - 4.0f, pos.y - halfH + 4.0f, pos.x - halfW, pos.y + halfH - 4.0f)) {
            vel.x = 0;
        }
    }

    SetPosition(pos);
    SetVelocity(vel);
}

void Player::RenderFrame(Graphics* graphics, Camera* camera) {
    if (!spriteSheet) return;

    // Get the frame slice from your controller
    RECT srcRect = anim.GetSourceRect();

    // 1. Calculate base position adjusted by camera
    D3DXVECTOR2 screenPos = GetPosition();
    if (camera) {
        screenPos.x -= camera->GetPosition().x;
        screenPos.y -= camera->GetPosition().y;
    }

    // 2. Build the transform matrix
    D3DXMATRIX scaleMat, transMat, finalMat;

    // Flip horizontal if moving left, and apply the uniform character zoom.
    float scaleX = (facingLeft ? -1.0f : 1.0f) * charScale;
    D3DXMatrixScaling(&scaleMat, scaleX, charScale, 1.0f);

    // Offset is the box-centre -> cell-top vector in cell space, so it scales with
    // the character to keep the feet planted as the sprite grows / shrinks.
    D3DXMatrixTranslation(&transMat, screenPos.x + spriteOffset.x * charScale, screenPos.y + spriteOffset.y * charScale, 0.0f);

    finalMat = scaleMat * transMat;

    // 3. Draw
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