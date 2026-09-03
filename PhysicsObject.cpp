#include "PhysicsObject.h"

PhysicsObject::PhysicsObject() {
    velocity = D3DXVECTOR2(0.0f, 0.0f);
    acceleration = D3DXVECTOR2(0.0f, 0.0f);
    force = D3DXVECTOR2(0.0f, 0.0f);

    mass = 1.0f;
    bounciness = 0.0f;
    friction = 0.0f;
    gravityScale = 1.0f;
}

void PhysicsObject::InitializePhysics(float m, float bounce, float drag, float gScale) {
    mass = m;
    bounciness = bounce;
    friction = drag;
    gravityScale = gScale;
}

void PhysicsObject::ApplyForce(D3DXVECTOR2 appliedForce) {
    force += appliedForce;
}

void PhysicsObject::ApplyGravity(D3DXVECTOR2 gravity) {
    // F = m * a. Gravity is a constant acceleration, so we apply it as a force.
    // The gravityScale allows you to make specific entities float or fall faster.
    force += gravity * mass * gravityScale;
}

void PhysicsObject::UpdateLogic(float deltaTime) {
    // 1. Calculate Acceleration (a = F / m)
    if (mass > 0.0f) {
        acceleration = force / mass;
    }

    // 2. Integrate Velocity (v = v0 + at)
    velocity += acceleration * deltaTime;

    // 3. Integrate Position (p = p0 + vt)
    D3DXVECTOR2 pos = GetPosition();
    pos += velocity * deltaTime;
    SetPosition(pos);

    // 4. Clear forces for the next frame
    force = D3DXVECTOR2(0.0f, 0.0f);
    acceleration = D3DXVECTOR2(0.0f, 0.0f);
}