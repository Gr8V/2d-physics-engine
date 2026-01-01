#pragma once
#include "math/Vec2.h"
#include "physics/colliders.h"

bool circleVsCircle(
    const Vec2& posA, const CircleCollider& a,
    const Vec2& posB, const CircleCollider& b
);

bool circleVsBox(
    const Vec2& circlePos, float radius,
    const Vec2& boxPos, const BoxCollider& box
);

bool AABBvsAABB(
    const Vec2& posA, const BoxCollider& A,
    const Vec2& posB, const BoxCollider& B
);