#include "physics/collisions.h"
#include "math/math_utils.h"

bool circleVsCircle(
    const Vec2& posA, const CircleCollider& a,
    const Vec2& posB, const CircleCollider& b
) {
    Vec2 delta = posB - posA;
    float distSq = delta.dot(delta);
    float radiusSum = a.radius + b.radius;
    return distSq <= radiusSum * radiusSum;
}

bool circleVsBox(
    const Vec2& circlePos, float radius,
    const Vec2& boxPos, const BoxCollider& box
) {
    float left   = boxPos.x - box.halfWidth;
    float right  = boxPos.x + box.halfWidth;
    float top    = boxPos.y - box.halfHeight;
    float bottom = boxPos.y + box.halfHeight;

    float closestX = clamp(circlePos.x, left, right);
    float closestY = clamp(circlePos.y, top, bottom);

    Vec2 closestPoint{closestX, closestY};
    Vec2 delta = circlePos - closestPoint;

    return delta.dot(delta) <= radius * radius;
}

bool AABBvsAABB(
    const Vec2& posA, const BoxCollider& A,
    const Vec2& posB, const BoxCollider& B
) {
    float leftA   = posA.x - A.halfWidth;
    float rightA  = posA.x + A.halfWidth;
    float topA    = posA.y - A.halfHeight;
    float bottomA = posA.y + A.halfHeight;

    float leftB   = posB.x - B.halfWidth;
    float rightB  = posB.x + B.halfWidth;
    float topB    = posB.y - B.halfHeight;
    float bottomB = posB.y + B.halfHeight;

    if (rightA  < leftB)   return false;
    if (leftA   > rightB)  return false;
    if (bottomA < topB)    return false;
    if (topA    > bottomB) return false;

    return true;
}
