#pragma once
#include "math/Vec2.h"

struct RigidBody {
    Vec2 position;
    Vec2 velocity;
    Vec2 force;

    float rotation = 0.f;          // radians
    float angularVelocity = 0.f;   // rad/s

    float mass;
    float invMass;

    float inertia = 0.f;
    float invInertia = 0.f;

    RigidBody(const Vec2& pos, float m);

    void applyForce(const Vec2& f);
    void integrate(float dt);
};
