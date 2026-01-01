#include "physics/physicsWorld.h"
#include "math/math_utils.h"
#include "physics/collisions.h"

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
                auto* cA = static_cast<CircleCollider*>(A.collider);
                auto* cB = static_cast<CircleCollider*>(B.collider);
                if (circleVsCircle(
                    A.body->position, *cA,
                    B.body->position, *cB))
                {
                    resolveCircleCircle(A, B);
                }
                
            }
            else if (A.type == ColliderType::Circle &&
                    B.type == ColliderType::Box)
            {
                auto* c = static_cast<CircleCollider*>(A.collider);
                auto* b = static_cast<BoxCollider*>(B.collider);

                if (circleVsBox(
                    A.body->position, c->radius,
                    B.body->position, *b))
                {
                    resolveCircleBox(A, B);
                }
            }
            else if (A.type == ColliderType::Box &&
                    B.type == ColliderType::Circle)
            {
                auto* c = static_cast<CircleCollider*>(B.collider);
                auto* b = static_cast<BoxCollider*>(A.collider);

                if (circleVsBox(
                        B.body->position, c->radius,
                        A.body->position, *b))
                {
                    resolveCircleBox(B, A); // swap for resolution too
                }
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

    // find distance between center of both circles
    Vec2 delta = B.body->position - A.body->position;
    float dist = delta.magnitude();

    // Normal
    Vec2 normal = (dist > 0.f) ? delta / dist : Vec2{1.f, 0.f};

    float penetration = (cA->radius + cB->radius) - dist;


    //calculates total inv mass to check if both bodies are static
    float invMassA = A.body->invMass;
    float invMassB = B.body->invMass;
    float totalInvMass = invMassA + invMassB;
    if (totalInvMass == 0.f)
        return;

    // positional correction
    Vec2 correction = normal * (penetration / totalInvMass);
    A.body->position -= correction * A.body->invMass;
    B.body->position += correction * B.body->invMass;

    // velocity correction (remove relative normal velocity)
    Vec2 relativeVelocity = B.body->velocity - A.body->velocity;
    float vn = relativeVelocity.dot(normal);

    if (vn >= 0.f)
    {
        return;
    }
    
    // bounciness
    float restitution = 0.5f;

    float j = -(1.f + restitution) * vn;
    j /= totalInvMass;

    Vec2 impulse = normal * j;
    A.body->velocity -= impulse * invMassA;
    B.body->velocity += impulse * invMassB;

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

    Vec2 normal;
    float penetration;

    if (dist > 0.f) {
        normal = delta / dist;
        penetration = circle->radius - dist;
    } else {
        // Circle center inside box
        normal = {0.f, -1.f};
        penetration = circle->radius;
    }

    float invMassC = circleObj.body->invMass;
    float invMassB = boxObj.body->invMass;
    float totalInvMass = invMassC + invMassB;

    if (totalInvMass == 0.f)
        return;

    // --- Position correction ---
    Vec2 correction = normal * (penetration / totalInvMass);
    circleObj.body->position += correction * invMassC;
    boxObj.body->position    -= correction * invMassB;

    // --- Velocity correction ---
    Vec2 rv = circleObj.body->velocity - boxObj.body->velocity;
    float vn = rv.dot(normal);

    if (vn >= 0.f)
        return;

    float restitution = 0.5f;

    float j = -(1.f + restitution) * vn;
    j /= totalInvMass;

    Vec2 impulse = normal * j;

    circleObj.body->velocity += impulse * invMassC;
    boxObj.body->velocity    -= impulse * invMassB;
}
