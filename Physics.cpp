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
        return CircleVsBox(b, a); // circle always first for correct calc
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

    return (aLeft < bRight && aRight > bLeft && aTop < bBottom && aBottom > bTop);
}

bool Physics::CircleVsBox(PhysicsObject* circle, PhysicsObject* box) {
    float testX = circle->GetPosition().x;
    float testY = circle->GetPosition().y;

    float boxLeft = box->GetPosition().x - (box->GetBoxWidth() / 2);
    float boxRight = box->GetPosition().x + (box->GetBoxWidth() / 2);
    float boxTop = box->GetPosition().y - (box->GetBoxHeight() / 2);
    float boxBottom = box->GetPosition().y + (box->GetBoxHeight() / 2);

    if (testX < boxLeft)  testX = boxLeft;
    else if (testX > boxRight) testX = boxRight;

    if (testY < boxTop)   testY = boxTop;
    else if (testY > boxBottom) testY = boxBottom;

    float distX = circle->GetPosition().x - testX;
    float distY = circle->GetPosition().y - testY;
    return (distX * distX) + (distY * distY) <= (circle->GetRadius() * circle->GetRadius());
}



void Physics::ResolveElasticCollision(PhysicsObject* a, PhysicsObject* b) {
    D3DXVECTOR2 normal;
    float penetration = 0.0f;

    ColliderType sa = a->GetColliderShape();
    ColliderType sb = b->GetColliderShape();

    if (sa == ColliderType::CIRCLE && sb == ColliderType::CIRCLE) {
        // Circle vs Circle: normal is centre-to-centre.
        D3DXVECTOR2 delta = a->GetPosition() - b->GetPosition();
        float dist = sqrtf(delta.x * delta.x + delta.y * delta.y);
        float combined = a->GetRadius() + b->GetRadius();
        if (dist >= combined) return;   // not actually overlapping

        if (dist == 0.0f) { normal = D3DXVECTOR2(1.0f, 0.0f); penetration = combined; }
        else { normal = delta / dist;             penetration = combined - dist; }
    }
    else if (sa == ColliderType::BOX && sb == ColliderType::BOX) {
        // Box vs Box: pick the axis of LEAST overlap and separate along it.
        D3DXVECTOR2 delta = a->GetPosition() - b->GetPosition();
        float overlapX = (a->GetBoxWidth() / 2.0f + b->GetBoxWidth() / 2.0f) - fabsf(delta.x);
        float overlapY = (a->GetBoxHeight() / 2.0f + b->GetBoxHeight() / 2.0f) - fabsf(delta.y);
        if (overlapX <= 0.0f || overlapY <= 0.0f) return;

        if (overlapX < overlapY) { normal = D3DXVECTOR2(delta.x < 0 ? -1.0f : 1.0f, 0.0f); penetration = overlapX; }
        else { normal = D3DXVECTOR2(0.0f, delta.y < 0 ? -1.0f : 1.0f); penetration = overlapY; }
    }
    else {
        // Circle vs Box: normalise so `circle` is one side and `box` is the other.
        PhysicsObject* circle = (sa == ColliderType::CIRCLE) ? a : b;
        PhysicsObject* box = (sa == ColliderType::CIRCLE) ? b : a;

        D3DXVECTOR2 cPos = circle->GetPosition();
        D3DXVECTOR2 bPos = box->GetPosition();
        float halfW = box->GetBoxWidth() / 2.0f;
        float halfH = box->GetBoxHeight() / 2.0f;
        float r = circle->GetRadius();

        // Closest point on the box to the circle centre.
        float cx = cPos.x, cy = cPos.y;
        if (cx < bPos.x - halfW) cx = bPos.x - halfW; else if (cx > bPos.x + halfW) cx = bPos.x + halfW;
        if (cy < bPos.y - halfH) cy = bPos.y - halfH; else if (cy > bPos.y + halfH) cy = bPos.y + halfH;

        D3DXVECTOR2 delta = cPos - D3DXVECTOR2(cx, cy);
        float dist = sqrtf(delta.x * delta.x + delta.y * delta.y);
        if (dist >= r) return;

        if (dist == 0.0f) { normal = D3DXVECTOR2(1.0f, 0.0f); penetration = r; }
        else { normal = delta / dist;             penetration = r - dist; }

        // The normal above points TOWARD the circle. Flip so it points from B to A.
        if (sa == ColliderType::BOX) normal = -normal;
    }


    float totalMass = a->GetMass() + b->GetMass();
    if (totalMass <= 0.0f) return;
    float ratioA = b->GetMass() / totalMass;
    float ratioB = a->GetMass() / totalMass;
    a->SetPosition(a->GetPosition() + normal * penetration * ratioA);
    b->SetPosition(b->GetPosition() - normal * penetration * ratioB);


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