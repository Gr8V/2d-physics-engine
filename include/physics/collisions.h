#pragma once
#include "math/Vec2.h"
#include "physics/colliders.h"

bool circleVsCircle(
    const Vec2& posA, const CircleCollider& a,
    const Vec2& posB, const CircleCollider& b
);
