#include "physics/physicsWorld.h"
#include "math/math_utils.h"
#include "physics/collisions.h"
#include <iostream>

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
                    resolveCircleVsCircle(A, B);
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
                    resolveCircleVsBox(A, B);
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
                    resolveCircleVsBox(B, A); // swap for resolution too
                }
            }
            else if (A.type == ColliderType::Box &&
                    B.type == ColliderType::Box)
            {
                auto* bA = static_cast<BoxCollider*>(A.collider);
                auto* bB = static_cast<BoxCollider*>(B.collider);

                if (AABBvsAABB(
                        A.body->position, *bA,
                        B.body->position, *bB))
                {
                    resolveAABBvsAABB(A, B);
                }
            }
        }
    }
}

void PhysicsWorld::resolveCircleVsCircle(
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

    if (penetration <= 0.f)
        return;

    //calculates total inv mass to check if both bodies are static
    float invMassA = A.body->invMass;
    float invMassB = B.body->invMass;
    float totalInvMass = invMassA + invMassB;
    if (totalInvMass == 0.f)
        return;

    // positional correction
    float correctionMag = std::max(penetration - penetrationSlop, 0.f)/ totalInvMass*penetrationPercent;

    Vec2 correction = normal * correctionMag;
    A.body->position -= correction * A.body->invMass;
    B.body->position += correction * B.body->invMass;

    // velocity correction (remove relative normal velocity)
    Vec2 rv = B.body->velocity - A.body->velocity;
    float vn = rv.dot(normal);

    if (vn >= 0.f)
    {
        return;
    }
    
    // bounciness
    float restitution = std::min(cA->restitution, cB->restitution);
    if (std::abs(vn) < 0.5f)
        restitution = 0.f;

    // IMPULSE
    float j = -(1.f + restitution) * vn;
    j /= totalInvMass;

    Vec2 impulse = normal * j;
    A.body->velocity -= impulse * invMassA;
    B.body->velocity += impulse * invMassB;
}


void PhysicsWorld::resolveCircleVsBox(
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

    if (penetration <= 0.f)
        return;

    float invMassC = circleObj.body->invMass;
    float invMassB = boxObj.body->invMass;
    float totalInvMass = invMassC + invMassB;

    if (totalInvMass == 0.f)
        return;

    // --- Position correction ---
    float correctionMag = std::max(penetration - penetrationSlop, 0.f)/ totalInvMass*penetrationPercent;

    Vec2 correction = normal * correctionMag;
    circleObj.body->position += correction * invMassC;
    boxObj.body->position    -= correction * invMassB;

    // --- Velocity correction ---
    Vec2 rv = circleObj.body->velocity - boxObj.body->velocity;
    float vn = rv.dot(normal);

    if (vn >= 0.f)
        return;

    // Bounciness
    float restitution = std::min(circle->restitution, box->restitution);
    if (std::abs(vn) < 0.5f)
        restitution = 0.f;

    // IMPULSE
    float j = -(1.f + restitution) * vn;
    j /= totalInvMass;

    Vec2 impulse = normal * j;

    circleObj.body->velocity += impulse * invMassC;
    boxObj.body->velocity    -= impulse * invMassB;
}


void PhysicsWorld::resolveAABBvsAABB(
    PhysicsObject& A,
    PhysicsObject& B)
{
    auto* cA = static_cast<BoxCollider*>(A.collider);
    auto* cB = static_cast<BoxCollider*>(B.collider);

    Vec2 posA = A.body->position;
    Vec2 posB = B.body->position;

    float dx = posB.x - posA.x;
    float px = (cA->halfWidth + cB->halfWidth) - std::abs(dx);

    if (px <= 0.f) return;

    float dy = posB.y - posA.y;
    float py = (cA->halfHeight + cB->halfHeight) - std::abs(dy);

    if (py <= 0.f) return;

    Vec2 normal;
    float penetration;

    if (px < py) {
        normal = { (dx < 0.f) ? -1.f : 1.f, 0.f };
        penetration = px;
    } else {
        normal = { 0.f, (dy < 0.f) ? -1.f : 1.f };
        penetration = py;
    }

    float invMassA = A.body->invMass;
    float invMassB = B.body->invMass;
    float totalInvMass = invMassA + invMassB;

    if (totalInvMass == 0.f)
        return;

    // --- Position correction ---
    float correctionMag = std::max(penetration - penetrationSlop, 0.f)/ totalInvMass*penetrationPercent;

    Vec2 correction = normal * correctionMag;
    A.body->position -= correction * invMassA;
    B.body->position += correction * invMassB;

    // --- Velocity correction ---
    Vec2 rv = B.body->velocity - A.body->velocity;
    float vn = rv.dot(normal);

    if (vn >= 0.f)
        return;

    // Bounciness
    float restitution = std::min(cA->restitution, cB->restitution);
    if (std::abs(vn) < 0.5f)
        restitution = 0.f;

    // IMPULSE
    float j = -(1.f + restitution) * vn;
    j /= totalInvMass;

    Vec2 impulse = normal * j;

    A.body->velocity -= impulse * invMassA;
    B.body->velocity += impulse * invMassB;
}