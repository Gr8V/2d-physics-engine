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

    void add(RigidBody* body, CircleCollider* collider);
    void add(RigidBody* body, BoxCollider* collider);

    void step(float dt);

private:
    std::vector<PhysicsObject> objects;

    void integrate(float dt);
    void solveCollisions();

    void resolveCircleCircle(PhysicsObject& A, PhysicsObject& B);
    void resolveCircleBox(PhysicsObject& circle, PhysicsObject& box);
};
