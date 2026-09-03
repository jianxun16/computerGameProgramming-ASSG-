#include "PhysicsObject.h"
#include <cmath>

PhysicsObject::PhysicsObject() : GameObject() {
    velocity = D3DXVECTOR2(0.0f, 0.0f);
    acceleration = D3DXVECTOR2(0.0f, 0.0f);
    force = D3DXVECTOR2(0.0f, 0.0f);

    // default setting as fallback
    mass = 1.0f;
    bounciness = 1.0f;
    friction = 0.95f;
    gravityScale = 1.0f; 

    colliderShape = ColliderType::CIRCLE;
    radius = 16.0f;
    boxWidth = 32.0f;
    boxHeight = 32.0f;
}

void PhysicsObject::InitializePhysics(float startMass, float bounce, float drag, float gScale) {
    mass = startMass;
    bounciness = bounce;
    friction = drag;
    gravityScale = gScale;
}

void PhysicsObject::ApplyForce(D3DXVECTOR2 appliedForce) {
    force += appliedForce;
}

void PhysicsObject::ApplyGravity(D3DXVECTOR2 globalGravity) {
    force += globalGravity * gravityScale;
}

void PhysicsObject::UpdateLogic(float deltaTime) {
    if (!active) return;

    if (mass > 0.0f) {
        acceleration = force / mass;
    }

    velocity += acceleration * deltaTime;

    velocity *= powf(friction, deltaTime * 60.0f);

    position += velocity * deltaTime;

    force = D3DXVECTOR2(0.0f, 0.0f);

    GameObject::UpdateLogic(deltaTime);
}