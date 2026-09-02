#include "Physics.h"
#include <cmath>
#include <algorithm>

bool Physics::CheckCollision(PhysicsObject* a, PhysicsObject* b) {
    if (!a->IsActive() || !b->IsActive()) return false;

    ColliderType shapeA = a->GetColliderShape();
    ColliderType shapeB = b->GetColliderShape();


    if (shapeA == ColliderType::CIRCLE && shapeB == ColliderType::CIRCLE) {
        return CircleVsCircle(a, b);
    }
    else if (shapeA == ColliderType::BOX && shapeB == ColliderType::BOX) {
        return BoxVsBox(a, b);
    }
    else if (shapeA == ColliderType::CIRCLE && shapeB == ColliderType::BOX) {
        return CircleVsBox(a, b);
    }
    else if (shapeA == ColliderType::BOX && shapeB == ColliderType::CIRCLE) {
        return CircleVsBox(b, a); // make sure circle always first for correct calc
    }

    return false;
}

bool Physics::CircleVsCircle(PhysicsObject* a, PhysicsObject* b) {
    D3DXVECTOR2 delta = a->GetPosition() - b->GetPosition();
    float distSquared = (delta.x * delta.x) + (delta.y * delta.y);
    float combinedRadius = a->GetRadius() + b->GetRadius();

    return distSquared <= (combinedRadius * combinedRadius);
}

bool Physics::BoxVsBox(PhysicsObject* a, PhysicsObject* b) {
    float aLeft = a->GetPosition().x - (a->GetBoxWidth() / 2.0f);
    float aRight = a->GetPosition().x + (a->GetBoxWidth() / 2.0f);
    float aTop = a->GetPosition().y - (a->GetBoxHeight() / 2.0f);
    float aBottom = a->GetPosition().y + (a->GetBoxHeight() / 2.0f);

    float bLeft = b->GetPosition().x - (b->GetBoxWidth() / 2.0f);
    float bRight = b->GetPosition().x + (b->GetBoxWidth() / 2.0f);
    float bTop = b->GetPosition().y - (b->GetBoxHeight() / 2.0f);
    float bBottom = b->GetPosition().y + (b->GetBoxHeight() / 2.0f);

    return (aLeft < bRight &&
        aRight > bLeft &&
        aTop < bBottom &&
        aBottom > bTop);
}

bool Physics::CircleVsBox(PhysicsObject* circle, PhysicsObject* box) {
    float testX = circle->GetPosition().x;
    float testY = circle->GetPosition().y;

    float boxLeft = box->GetPosition().x - (box->GetBoxWidth() / 2);
    float boxRight = box->GetPosition().x + (box->GetBoxWidth() / 2);
    float boxTop = box->GetPosition().y - (box->GetBoxHeight() / 2);
    float boxBottom = box->GetPosition().y + (box->GetBoxHeight() / 2);

    if (testX < boxLeft) testX = boxLeft;
    else if (testX > boxRight) testX = boxRight;

    if (testY < boxTop) testY = boxTop;
    else if (testY > boxBottom) testY = boxBottom;

    float distX = circle->GetPosition().x - testX;
    float distY = circle->GetPosition().y - testY;
    float distanceSquared = (distX * distX) + (distY * distY);

    return distanceSquared <= (circle->GetRadius() * circle->GetRadius());
}

void Physics::ResolveElasticCollision(PhysicsObject* a, PhysicsObject* b) {
    D3DXVECTOR2 delta = a->GetPosition() - b->GetPosition();
    float distance = sqrt((delta.x * delta.x) + (delta.y * delta.y));

    if (distance == 0.0f) {
        delta = D3DXVECTOR2(1.0f, 0.0f);
        distance = 1.0f;
    }

    float avgRadiusA = (a->GetColliderShape() == ColliderType::CIRCLE) ? a->GetRadius() : (a->GetBoxWidth() / 2.0f);
    float avgRadiusB = (b->GetColliderShape() == ColliderType::CIRCLE) ? b->GetRadius() : (b->GetBoxWidth() / 2.0f);

    // resolve overlap
    float overlap = (avgRadiusA + avgRadiusB) - distance;
    if (overlap < 0.0f) overlap = 0.1f; 

    D3DXVECTOR2 normal = delta / distance;

    float totalMass = a->GetMass() + b->GetMass();
    float ratioA = b->GetMass() / totalMass;
    float ratioB = a->GetMass() / totalMass;

    a->SetPosition(a->GetPosition() + (normal * overlap * ratioA));
    b->SetPosition(b->GetPosition() - (normal * overlap * ratioB));


    D3DXVECTOR2 relativeVelocity = a->GetVelocity() - b->GetVelocity();
    float speedAlongNormal = (relativeVelocity.x * normal.x) + (relativeVelocity.y * normal.y);

    if (speedAlongNormal > 0) return;

    float bounce = min(a->GetBounciness(), b->GetBounciness());

    float impulse = -(1.0f + bounce) * speedAlongNormal;
    impulse /= (1.0f / a->GetMass()) + (1.0f / b->GetMass());

    D3DXVECTOR2 impulseVector = normal * impulse;
    a->SetVelocity(a->GetVelocity() + (impulseVector / a->GetMass()));
    b->SetVelocity(b->GetVelocity() - (impulseVector / b->GetMass()));
}