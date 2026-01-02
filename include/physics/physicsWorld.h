#pragma once
#include <vector>
#include "physics/rigidBody.h"
#include "physics/colliders.h"

enum class ColliderType {
    Circle,
    Box
};

struct PhysicsObject {
    RigidBody* body;
    ColliderType type;
    void* collider;
};

class PhysicsWorld {
public:
    Vec2 gravity = {0.f, 9.81f};

    float penetrationPercent = 0.8f; // Baumgarte factor
    float penetrationSlop    = 0.01f;

    void add(RigidBody* body, CircleCollider* collider);
    void add(RigidBody* body, BoxCollider* collider);

    void step(float dt);

private:
    std::vector<PhysicsObject> objects;

    void integrate(float dt);
    void solveCollisions();

    void resolveCircleVsCircle(PhysicsObject& A, PhysicsObject& B);
    void resolveCircleVsBox(PhysicsObject& circle, PhysicsObject& box);
    void resolveAABBvsAABB( PhysicsObject& A, PhysicsObject& B);
};
