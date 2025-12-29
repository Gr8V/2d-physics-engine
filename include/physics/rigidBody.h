#pragma once
#include "math/Vec2.h"

struct RigidBody {
    Vec2 position;
    Vec2 velocity;
    Vec2 force;

    float mass;
    float invMass;

    RigidBody(const Vec2& pos, float m);

    void applyForce(const Vec2& f);
    void integrate(float dt);
};
