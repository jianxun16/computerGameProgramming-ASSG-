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

    // Vector from the collider-box CENTRE (= GetPosition()) to the sprite's
    // top-left corner. The old art puts the character's feet at (100, 137)
    // inside a 192px cell, and the collider reaches BASE_BOX_H/2 = 32px above
    // the feet, so the feet must land on the box BOTTOM (the ground contact):
    //   x: -FEET_CENTER_X               = -100  (feet on the box centre-line)
    //   y: -(FEET_Y - BASE_BOX_H / 2)   = -105  (feet on the box bottom = ground)
    // Using -137 (the old -FEET_Y) instead put the feet on the box CENTRE,
    // floating the warrior ~32px above the tiles.
    spriteOffset = D3DXVECTOR2(-100.0f, -105.0f);
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
    // Warrior_Anim.png is a 1024x1024 canvas, but the art is authored on a 5x5
    // grid of 192px cells (only the top-left 960x960 is used; the rest is
    // margin). Pass the USED area so the controller derives 192px frames, NOT
    // 1024/5 = 204 - otherwise every source rect drifts and the feet ride up.
    // The animations are BLOCKS of cells, not one row each, so we drive it in
    // clip mode (see ChangeState for the idle / run / attack blocks).
    anim.SetupSheet(960, 960, 5, 5, 0.1f);
    ChangeState(IDLE);
}

void Player::ChangeState(AnimState newState) {
    if (currentState == newState) return;

    currentState = newState;

    // Frame blocks inside Warrior_Anim.png (5-column grid), read left->right,
    // top->bottom. Run flips horizontally at draw time, so left/right share one
    // block. These match the sheet layout: idle spans rows 0-2, attack sits in
    // columns 3-4, run spans rows 3-4.
    switch (newState) {
    case IDLE:   anim.PlayClip(AnimClip{ 0, 0, 3, 8 }); break; // 8 idle frames
    case RUN:    anim.PlayClip(AnimClip{ 0, 3, 3, 6 }); break; // 6 run frames
    case ATTACK: anim.PlayClip(AnimClip{ 3, 0, 2, 4 }); break; // 4 attack frames
    }
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

    // Point the Sprite at the current animation frame of the sheet.
    sprite.SetTexture(spriteSheet);
    sprite.SetSourceRect(anim.GetSourceRect());

    // Flip horizontally when facing left, and apply the uniform character zoom.
    // A negative scale.x mirrors the sprite around its (top-left) origin.
    float scaleX = (facingLeft ? -1.0f : 1.0f) * charScale;
    D3DXVECTOR2 scl(scaleX, charScale);

    // spriteOffset is the box-centre -> sprite top-left vector; it scales with
    // the character so the feet stay planted as the warrior grows / shrinks.
    // We add it in world space and let the camera do the scrolling.
    D3DXVECTOR2 worldPos = GetPosition();
    worldPos.x += spriteOffset.x * charScale;
    worldPos.y += spriteOffset.y * charScale;

    sprite.Draw(graphics, camera, worldPos, scl);
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