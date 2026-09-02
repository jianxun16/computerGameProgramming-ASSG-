#pragma once
#include "GameObject.h"

enum class ColliderType {
    CIRCLE,
    BOX
};

class PhysicsObject : public GameObject {
private:
    D3DXVECTOR2 velocity;
    D3DXVECTOR2 acceleration;
    D3DXVECTOR2 force;

    float mass;
    float bounciness;   
    float friction;    
    float gravityScale;

    ColliderType colliderShape;
    float boxWidth;
    float boxHeight;
    float radius;

public:
    PhysicsObject();
    virtual ~PhysicsObject() {}

    // setup properties
    void InitializePhysics(float mass, float bounce, float drag, float gScale);

    void ApplyForce(D3DXVECTOR2 appliedForce);
    void ApplyGravity(D3DXVECTOR2 gravity);

    void UpdateLogic(float deltaTime) override;

    D3DXVECTOR2 GetVelocity() const { return velocity; }
    void SetVelocity(D3DXVECTOR2 vel) { velocity = vel; }
    void SetMass(float m) { mass = m; }
    void SetBounciness(float b) { bounciness = b; }
    void SetFriction(float f) { friction = f; }
    void SetColliderCircle(float r) { colliderShape = ColliderType::CIRCLE; radius = r; }
    void SetColliderBox(float w, float h) { colliderShape = ColliderType::BOX; boxWidth = w; boxHeight = h; }

    float GetMass() const { return mass; }
    float GetBounciness() const { return bounciness; }
    float GetFriction() const { return friction; }
    ColliderType GetColliderShape() const { return colliderShape; }
    float GetBoxWidth() const { return boxWidth; }
    float GetBoxHeight() const { return boxHeight; }
    float GetRadius() const { return radius; }
};