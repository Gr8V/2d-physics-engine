#pragma once
#include <Vec2.h>
#include <colliders.h>

bool circleVsCircle(
    const Vec2& posA, const CircleCollider& a,
    const Vec2& posB, const CircleCollider& b
);
