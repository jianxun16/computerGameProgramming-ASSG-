#include "PhysicsObject.h"

PhysicsObject::PhysicsObject() : GameObject() {
    velocity = D3DXVECTOR2(0.0f, 0.0f);
    acceleration = D3DXVECTOR2(0.0f, 0.0f);
    force = D3DXVECTOR2(0.0f, 0.0f);

    // Physics defaults
    mass = 1.0f;
    bounciness = 1.0f;
    friction = 0.95f;
    gravityScale = 1.0f; // Default to falling normally

    // Collider defaults (defaults to a 32x32 circle)
    colliderShape = ColliderType::CIRCLE;
    radius = 16.0f;
    boxWidth = 32.0f;
    boxHeight = 32.0f;
}

// Updated: Removed 'colRadius', added 'gScale'
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
    velocity *= friction;
    position += velocity * deltaTime;

    force = D3DXVECTOR2(0.0f, 0.0f);

    GameObject::UpdateLogic(deltaTime);
}