#pragma once
#include "math/Vec2.h"

struct Body {
    Vec2 position;
    Vec2 velocity;
    Vec2 force;

    float mass;
    float invMass;

    Body(const Vec2& pos, float m);

    void applyForce(const Vec2& f);
    void integrate(float dt);
};
