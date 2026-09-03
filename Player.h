#pragma once
#include "PhysicsObject.h"
#include "AnimationController.h"
#include "TileMap.h"
#include "Input.h"
#include "Graphics.h"
#include "Camera.h"
#include "AudioManager.h"
#include "Sprite.h"

class Player : public PhysicsObject {
private:
    AnimationController anim;
    LPDIRECT3DTEXTURE9 spriteSheet;
    Sprite sprite;   // reusable renderer for the warrior sprite sheet

    // Player specific stats
    float moveSpeed;
    float jumpForce;
    bool isJumping;
    bool facingLeft;
    bool isAttacking;
    float attackTimer;   // seconds left in the current one-shot attack

    // Base collider size at scale 1.0; the live box = base * charScale.
    static const int BASE_BOX_W = 32;
    static const int BASE_BOX_H = 64;
    float charScale;     // uniform character zoom (item pickups resize this)


    // Animation Track IDs based on your team's old sprite sheet
    enum AnimState { IDLE = 0, RUN = 1, ATTACK = 2 };
    AnimState currentState;

    void ChangeState(AnimState newState);

public:
    Player();
    ~Player();

    void Initialize(Graphics* graphics, D3DXVECTOR2 startPos);
    void UpdateLogic(Input* input, float deltaTime, AudioManager* audio);

    // Kept separate from UpdateLogic so the PlayState can control the execution order
    void ResolveMapCollisions(TileMap* map);

    void RenderFrame(Graphics* graphics, Camera* camera) override;   // overrides GameObject::RenderFrame

    // ---- World-space helpers used by the gameplay states ----
    // Position is the CENTRE of the collider box, so feet = centre + halfHeight.
    float GetFeetY() const { return GetPosition().y + GetBoxHeight() / 2.0f; }
    float GetFeetWorldX() const { return GetPosition().x; }
    void  GetWorldHitbox(float& left, float& top, float& right, float& bottom) const;

    // Attack range: a circle in front of the player, active only while a one-shot
    // attack is playing. Returns true and fills centre (world space) + radius when
    // active, so a state can test hits against enemies (e.g. the boss).
    bool  GetAttackCircleWorld(D3DXVECTOR2& centre, float& radius) const;

    // Uniform character zoom (item pickups shrink / grow the warrior). Feet stay
    // planted, and the collider box scales with it.
    void  SetScale(float s);
    float GetScale() const { return charScale; }
};