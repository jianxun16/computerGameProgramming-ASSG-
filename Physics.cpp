#include "Physics.h"
#include <cmath>
#include <algorithm>

// -----------------------------------------------------------------------------
// Collision detection (unchanged: bool tests only)
// -----------------------------------------------------------------------------

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
    float aLeft   = a->GetPosition().x - (a->GetBoxWidth()  / 2.0f);
    float aRight  = a->GetPosition().x + (a->GetBoxWidth()  / 2.0f);
    float aTop    = a->GetPosition().y - (a->GetBoxHeight() / 2.0f);
    float aBottom = a->GetPosition().y + (a->GetBoxHeight() / 2.0f);

    float bLeft   = b->GetPosition().x - (b->GetBoxWidth()  / 2.0f);
    float bRight  = b->GetPosition().x + (b->GetBoxWidth()  / 2.0f);
    float bTop    = b->GetPosition().y - (b->GetBoxHeight() / 2.0f);
    float bBottom = b->GetPosition().y + (b->GetBoxHeight() / 2.0f);

    return (aLeft < bRight &&
        aRight > bLeft &&
        aTop < bBottom &&
        aBottom > bTop);
}

bool Physics::CircleVsBox(PhysicsObject* circle, PhysicsObject* box) {
    float testX = circle->GetPosition().x;
    float testY = circle->GetPosition().y;

    float boxLeft   = box->GetPosition().x - (box->GetBoxWidth()  / 2);
    float boxRight  = box->GetPosition().x + (box->GetBoxWidth()  / 2);
    float boxTop    = box->GetPosition().y - (box->GetBoxHeight() / 2);
    float boxBottom = box->GetPosition().y + (box->GetBoxHeight() / 2);

    if (testX < boxLeft)  testX = boxLeft;
    else if (testX > boxRight) testX = boxRight;

    if (testY < boxTop)   testY = boxTop;
    else if (testY > boxBottom) testY = boxBottom;

    float distX = circle->GetPosition().x - testX;
    float distY = circle->GetPosition().y - testY;
    float distanceSquared = (distX * distX) + (distY * distY);

    return distanceSquared <= (circle->GetRadius() * circle->GetRadius());
}


// -----------------------------------------------------------------------------
// Shape-aware collision info used by ResolveElasticCollision.
//   normal      = unit vector pointing from B toward A (used to push A away)
//   penetration = how deep they overlap along `normal` (0 means just touching)
//   valid       = false means "not actually colliding, don't resolve"
// -----------------------------------------------------------------------------

struct CollisionInfo {
    D3DXVECTOR2 normal;
    float penetration;
    bool valid;
};

// Circle vs Circle: normal is centre-to-centre, penetration is R1+R2 - distance.
static CollisionInfo InfoCircleCircle(PhysicsObject* a, PhysicsObject* b) {
    CollisionInfo info; info.valid = false;
    D3DXVECTOR2 delta = a->GetPosition() - b->GetPosition();
    float distSq = delta.x * delta.x + delta.y * delta.y;
    float combined = a->GetRadius() + b->GetRadius();
    if (distSq > combined * combined) return info;

    float dist = sqrtf(distSq);
    if (dist == 0.0f) {
        // Centres coincide, pick any axis to break the tie.
        info.normal = D3DXVECTOR2(1.0f, 0.0f);
        info.penetration = combined;
    }
    else {
        info.normal = delta / dist;
        info.penetration = combined - dist;
    }
    info.valid = true;
    return info;
}

// Box vs Box: Minimum Translation Vector (MTV).
// Take overlap on each axis; separate along the axis of LEAST overlap.
// This is the correct way to resolve AABB collisions (matches the way a
// tall box hit on top gets pushed up, not sideways).
static CollisionInfo InfoBoxBox(PhysicsObject* a, PhysicsObject* b) {
    CollisionInfo info; info.valid = false;

    D3DXVECTOR2 aPos = a->GetPosition();
    D3DXVECTOR2 bPos = b->GetPosition();
    float aHalfW = a->GetBoxWidth()  / 2.0f;
    float aHalfH = a->GetBoxHeight() / 2.0f;
    float bHalfW = b->GetBoxWidth()  / 2.0f;
    float bHalfH = b->GetBoxHeight() / 2.0f;

    float dx = aPos.x - bPos.x;
    float dy = aPos.y - bPos.y;

    float overlapX = (aHalfW + bHalfW) - fabsf(dx);
    float overlapY = (aHalfH + bHalfH) - fabsf(dy);
    if (overlapX <= 0.0f || overlapY <= 0.0f) return info;

    if (overlapX < overlapY) {
        info.normal = D3DXVECTOR2(dx < 0.0f ? -1.0f : 1.0f, 0.0f);
        info.penetration = overlapX;
    }
    else {
        info.normal = D3DXVECTOR2(0.0f, dy < 0.0f ? -1.0f : 1.0f);
        info.penetration = overlapY;
    }
    info.valid = true;
    return info;
}

// Circle vs Box: closest point on box to circle centre gives the collision
// normal; if the centre is INSIDE the box, push out along the nearest edge.
static CollisionInfo InfoCircleBox(PhysicsObject* circle, PhysicsObject* box) {
    CollisionInfo info; info.valid = false;

    D3DXVECTOR2 cPos = circle->GetPosition();
    D3DXVECTOR2 bPos = box->GetPosition();
    float bHalfW = box->GetBoxWidth()  / 2.0f;
    float bHalfH = box->GetBoxHeight() / 2.0f;
    float r = circle->GetRadius();

    // Clamp the circle centre onto the box to find the closest point.
    float closestX = cPos.x;
    float closestY = cPos.y;
    if (closestX < bPos.x - bHalfW) closestX = bPos.x - bHalfW;
    else if (closestX > bPos.x + bHalfW) closestX = bPos.x + bHalfW;
    if (closestY < bPos.y - bHalfH) closestY = bPos.y - bHalfH;
    else if (closestY > bPos.y + bHalfH) closestY = bPos.y + bHalfH;

    float dx = cPos.x - closestX;
    float dy = cPos.y - closestY;
    float distSq = dx * dx + dy * dy;
    if (distSq > r * r) return info;

    float dist = sqrtf(distSq);
    if (dist == 0.0f) {
        // Circle centre inside the box: push out along the nearest edge.
        float leftD  = fabsf(cPos.x - (bPos.x - bHalfW));
        float rightD = fabsf((bPos.x + bHalfW) - cPos.x);
        float topD   = fabsf(cPos.y - (bPos.y - bHalfH));
        float botD   = fabsf((bPos.y + bHalfH) - cPos.y);
        float mn = leftD; info.normal = D3DXVECTOR2(-1.0f, 0.0f);
        if (rightD < mn) { mn = rightD; info.normal = D3DXVECTOR2(1.0f, 0.0f); }
        if (topD   < mn) { mn = topD;   info.normal = D3DXVECTOR2(0.0f, -1.0f); }
        if (botD   < mn) { mn = botD;   info.normal = D3DXVECTOR2(0.0f, 1.0f); }
        info.penetration = mn + r;
    }
    else {
        info.normal = D3DXVECTOR2(dx / dist, dy / dist);
        info.penetration = r - dist;
    }
    info.valid = true;
    return info;
}


// -----------------------------------------------------------------------------
// Elastic response: separate along the correct normal, apply impulse.
// -----------------------------------------------------------------------------

void Physics::ResolveElasticCollision(PhysicsObject* a, PhysicsObject* b) {
    // Pick the right normal / penetration for the shape combination.
    ColliderType sa = a->GetColliderShape();
    ColliderType sb = b->GetColliderShape();
    CollisionInfo info;

    if (sa == ColliderType::CIRCLE && sb == ColliderType::CIRCLE) {
        info = InfoCircleCircle(a, b);
    }
    else if (sa == ColliderType::BOX && sb == ColliderType::BOX) {
        info = InfoBoxBox(a, b);
    }
    else if (sa == ColliderType::CIRCLE && sb == ColliderType::BOX) {
        info = InfoCircleBox(a, b);
    }
    else { // BOX vs CIRCLE: run helper with circle first, then flip normal
        info = InfoCircleBox(b, a);
        info.normal = -info.normal;
    }

    // Not actually overlapping? Do nothing. (Fixes the old "overlap = 0.1f"
    // fallback which pushed objects apart even when they weren't colliding.)
    if (!info.valid) return;

    float totalMass = a->GetMass() + b->GetMass();
    if (totalMass <= 0.0f) return;

    // Positional correction: move each body along the normal, weighted by
    // the OTHER body's mass share (heavier things move less).
    float ratioA = b->GetMass() / totalMass;
    float ratioB = a->GetMass() / totalMass;
    a->SetPosition(a->GetPosition() + (info.normal * info.penetration * ratioA));
    b->SetPosition(b->GetPosition() - (info.normal * info.penetration * ratioB));

    // Impulse: only apply if they're actually closing along the normal.
    D3DXVECTOR2 relativeVelocity = a->GetVelocity() - b->GetVelocity();
    float speedAlongNormal = (relativeVelocity.x * info.normal.x) + (relativeVelocity.y * info.normal.y);
    if (speedAlongNormal > 0) return;

    float bounce = min(a->GetBounciness(), b->GetBounciness());
    float impulse = -(1.0f + bounce) * speedAlongNormal;
    impulse /= (1.0f / a->GetMass()) + (1.0f / b->GetMass());

    D3DXVECTOR2 impulseVector = info.normal * impulse;
    a->SetVelocity(a->GetVelocity() + (impulseVector / a->GetMass()));
    b->SetVelocity(b->GetVelocity() - (impulseVector / b->GetMass()));
}