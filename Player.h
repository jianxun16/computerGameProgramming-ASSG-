#pragma once
#include "PhysicsObject.h"
#include "AnimationController.h"
#include "TileMap.h"
#include "Input.h"
#include "Graphics.h"
#include "Camera.h"
#include "AudioManager.h"

class Player : public PhysicsObject {
private:
    AnimationController anim;
    LPDIRECT3DTEXTURE9 spriteSheet;

    // Player specific stats
    float moveSpeed;
    float jumpForce;
    bool isJumping;
    bool facingLeft;
    bool isAttacking;

    // Sprite offset tuning
    D3DXVECTOR2 spriteOffset;

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

    void RenderFrame(Graphics* graphics, Camera* camera);
};