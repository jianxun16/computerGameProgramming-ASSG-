#pragma once
#include <d3dx9.h>
#include "PhysicsObject.h"

class Physics {
public:
    static bool CheckCollision(PhysicsObject* a, PhysicsObject* b);
    static void ResolveElasticCollision(PhysicsObject* a, PhysicsObject* b);

private:
    static bool CircleVsCircle(PhysicsObject* a, PhysicsObject* b);
    static bool BoxVsBox(PhysicsObject* a, PhysicsObject* b);
    static bool CircleVsBox(PhysicsObject* circle, PhysicsObject* box);
};