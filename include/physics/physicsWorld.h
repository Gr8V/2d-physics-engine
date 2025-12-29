#pragma once
#include <vector>
#include "physics/RigidBody.h"
#include "physics/colliders.h"

struct PhysicsObject {
    RigidBody* body;
    CircleCollider* collider;
};

class PhysicsWorld {
public:
    Vec2 gravity = {0.f, 9.81f};

    void add(RigidBody* body, CircleCollider* collider);
    void step(float dt);

private:
    std::vector<PhysicsObject> objects;

    void integrate(float dt);
    void solveCollisions();
};
