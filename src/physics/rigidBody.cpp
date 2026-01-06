#include "physics/rigidBody.h"
#include "math/math_utils.h"

RigidBody::RigidBody(const Vec2& pos, float m)
    : position(pos), velocity(0, 0), force(0, 0), mass(m)
{
    invMass = (mass > 0.f) ? 1.f / mass : 0.f;
}

void RigidBody::applyForce(const Vec2& f)
{
    force += f;
}

void RigidBody::applyImpulse(
    const Vec2& impulse,
    const Vec2& contactVector
) {
    if (invMass == 0.f) return;

    velocity += impulse * invMass;
    angularVelocity += cross(contactVector, impulse) * invInertia;
}

void RigidBody::integrate(float dt)
{
    if (invMass == 0.f) return; // static body

    // a = F / m
    Vec2 acceleration = force * invMass;

    // semi-implicit Euler (good default)
    velocity += acceleration * dt;
    position += velocity * dt;
    rotation += angularVelocity * dt;

    // clear forces
    force = {0, 0};
}
