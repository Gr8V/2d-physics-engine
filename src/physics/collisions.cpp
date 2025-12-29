#include <collisions.h>

bool circleVsCircle(
    const Vec2& posA, const CircleCollider& a,
    const Vec2& posB, const CircleCollider& b
) {
    Vec2 delta = posB - posA;
    float distSq = delta.dot(delta);
    float radiusSum = a.radius + b.radius;
    return distSq <= radiusSum * radiusSum;
}