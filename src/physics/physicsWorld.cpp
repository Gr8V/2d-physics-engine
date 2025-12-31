#include "physics/physicsWorld.h"
#include "math/math_utils.h"

void PhysicsWorld::add(RigidBody* body, CircleCollider* collider)
{
    objects.push_back({ body, ColliderType::Circle, collider });
}

void PhysicsWorld::add(RigidBody* body, BoxCollider* collider)
{
    objects.push_back({ body, ColliderType::Box, collider });
}

void PhysicsWorld::step(float dt)
{
    integrate(dt);
    for (int k = 0; k < 4; k++)
        solveCollisions();

}

void PhysicsWorld::integrate(float dt)
{
    for (auto& obj : objects) {
        if (obj.body->invMass == 0.f) continue;

        obj.body->applyForce(gravity * obj.body->mass);
        obj.body->integrate(dt);
    }
}

void PhysicsWorld::solveCollisions()
{
    for (size_t i = 0; i < objects.size(); i++) {
        for (size_t j = i + 1; j < objects.size(); j++) {

            auto& A = objects[i];
            auto& B = objects[j];

            if (A.type == ColliderType::Circle &&
                B.type == ColliderType::Circle)
            {
                resolveCircleCircle(A, B);
            }
            else if (A.type == ColliderType::Circle &&
                    B.type == ColliderType::Box)
            {
                resolveCircleBox(A, B);
            }
            else if (A.type == ColliderType::Box &&
                    B.type == ColliderType::Circle)
            {
                resolveCircleBox(B, A); // swap
            }
        }
    }
}

void PhysicsWorld::resolveCircleCircle(
    PhysicsObject& A,
    PhysicsObject& B)
{
    auto* cA = static_cast<CircleCollider*>(A.collider);
    auto* cB = static_cast<CircleCollider*>(B.collider);

    Vec2 delta = B.body->position - A.body->position;
    float dist = delta.magnitude();

    float penetration = (cA->radius + cB->radius) - dist;
    if (penetration <= 0.f)
        return;

    Vec2 normal = (dist > 0.f) ? delta / dist : Vec2{1.f, 0.f};

    float totalInvMass = A.body->invMass + B.body->invMass;
    if (totalInvMass == 0.f)
        return;

    // positional correction
    Vec2 correction = normal * (penetration / totalInvMass);
    A.body->position -= correction * A.body->invMass;
    B.body->position += correction * B.body->invMass;

    // velocity correction (remove relative normal velocity)
    Vec2 relativeVelocity = B.body->velocity - A.body->velocity;
    float vn = relativeVelocity.dot(normal);

    // Only correct if moving toward each other
    if (vn < 0.f) {
        Vec2 impulse = normal * (vn / totalInvMass);
        A.body->velocity += impulse * A.body->invMass;
        B.body->velocity -= impulse * B.body->invMass;
    }
}


void PhysicsWorld::resolveCircleBox(
    PhysicsObject& circleObj,
    PhysicsObject& boxObj)
{
    auto* circle = static_cast<CircleCollider*>(circleObj.collider);
    auto* box    = static_cast<BoxCollider*>(boxObj.collider);

    Vec2 cPos = circleObj.body->position;
    Vec2 bPos = boxObj.body->position;

    float left   = bPos.x - box->halfWidth;
    float right  = bPos.x + box->halfWidth;
    float top    = bPos.y - box->halfHeight;
    float bottom = bPos.y + box->halfHeight;

    float closestX = clamp(cPos.x, left, right);
    float closestY = clamp(cPos.y, top, bottom);

    Vec2 closestPoint{ closestX, closestY };
    Vec2 delta = cPos - closestPoint;

    float dist = delta.magnitude();
    if (dist >= circle->radius)
        return;

    Vec2 normal;
    float penetration;

    if (dist == 0.f) {
        normal = {0.f, -1.f}; // floor assumption
        penetration = circle->radius;
    } else {
        normal = delta / dist;
        penetration = circle->radius - dist;
    }

    // positional correction (remove overlap)
    circleObj.body->position += normal * penetration;
    
    // cancel velocity INTO the surface
    float vn = circleObj.body->velocity.dot(normal);

    if (vn < 0.f) {
        float restitution = 0.8f; // try 0.5–1.0
        circleObj.body->velocity -= normal * (1.f + restitution) * vn;
    }

}
