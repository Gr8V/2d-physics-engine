#include "physics/physicsWorld.h"
#include "math/math_utils.h"
#include "physics/collisions.h"
#include <algorithm>
#include <cmath>
#include <iostream>

void PhysicsWorld::add(RigidBody* body, CircleCollider* collider)
{
    // --- inertia for circle ---
    if (body->invMass == 0.f) {
        body->inertia = 0.f;
        body->invInertia = 0.f;
    } else {
        float r = collider->radius;
        body->inertia = 0.5f * body->mass * r * r;
        body->invInertia = 1.f / body->inertia;
    }

    objects.push_back({ body, ColliderType::Circle, collider });
}

void PhysicsWorld::add(RigidBody* body, BoxCollider* collider)
{
    // --- inertia for box ---
    if (body->invMass == 0.f) {
        body->inertia = 0.f;
        body->invInertia = 0.f;
    } else {
        float w = collider->halfWidth * 2.f;
        float h = collider->halfHeight * 2.f;
        body->inertia = (1.f / 12.f) * body->mass * (w*w + h*h);
        body->invInertia = 1.f / body->inertia;
    }

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

    Vec2 delta = B.body->position - A.body->position;
    float dist = delta.magnitude();
    if (dist <= 0.f) return;

    Vec2 normal = delta / dist;
    float penetration = (cA->radius + cB->radius) - dist;
    if (penetration <= 0.f) return;

    float invMassA = A.body->invMass;
    float invMassB = B.body->invMass;
    float totalInvMass = invMassA + invMassB;
    if (totalInvMass == 0.f) return;

    // -------- CONTACT POINT --------
    Vec2 contactPoint =
        A.body->position + normal * cA->radius;

    Vec2 rA = contactPoint - A.body->position;
    Vec2 rB = contactPoint - B.body->position;

    // -------- RELATIVE VELOCITY (WITH ROTATION) --------
    Vec2 velA = A.body->velocity +
                perp(rA) * A.body->angularVelocity;

    Vec2 velB = B.body->velocity +
                perp(rB) * B.body->angularVelocity;

    Vec2 rv = velB - velA;
    float vn = rv.dot(normal);

    // -------- NORMAL IMPULSE --------
    float j = 0.f;
    if (vn < 0.f) {
        float restitution = std::min(cA->restitution, cB->restitution);
        if (std::abs(vn) < 0.3f) restitution = 0.f;

        float rnA = cross(rA, normal);
        float rnB = cross(rB, normal);

        float denom =
            invMassA + invMassB +
            rnA * rnA * A.body->invInertia +
            rnB * rnB * B.body->invInertia;

        j = -(1.f + restitution) * vn / denom;

        Vec2 impulse = normal * j;
        A.body->velocity -= impulse * invMassA;
        B.body->velocity += impulse * invMassB;
    }

    // -------- FRICTION --------
    Vec2 velA2 = A.body->velocity +
                 perp(rA) * A.body->angularVelocity;

    Vec2 velB2 = B.body->velocity +
                 perp(rB) * B.body->angularVelocity;

    Vec2 rv2 = velB2 - velA2;
    float vn2 = rv2.dot(normal);

    Vec2 tangent = rv2 - normal * vn2;
    float tMag = tangent.magnitude();

    if (tMag > 1e-4f) {
        tangent /= tMag;

        float jt = -rv2.dot(tangent);

        float rtA = cross(rA, tangent);
        float rtB = cross(rB, tangent);

        float denomT =
            invMassA + invMassB +
            rtA * rtA * A.body->invInertia +
            rtB * rtB * B.body->invInertia;

        jt /= denomT;

        float muS = std::sqrt(
            cA->staticFriction * cA->staticFriction +
            cB->staticFriction * cB->staticFriction);

        float muD = std::sqrt(
            cA->dynamicFriction * cA->dynamicFriction +
            cB->dynamicFriction * cB->dynamicFriction);

        Vec2 frictionImpulse;
        if (std::abs(jt) < j * muS)
            frictionImpulse = tangent * jt;
        else
            frictionImpulse = tangent * -j * muD;

        A.body->velocity -= frictionImpulse * invMassA;
        B.body->velocity += frictionImpulse * invMassB;

        // 🔥 TORQUE FROM FRICTION
        A.body->angularVelocity -=
            cross(rA, frictionImpulse) * A.body->invInertia;
        B.body->angularVelocity +=
            cross(rB, frictionImpulse) * B.body->invInertia;
    }

    // -------- POSITION CORRECTION --------
    float correctionMag =
        std::max(penetration - penetrationSlop, 0.f) /
        totalInvMass * penetrationPercent;

    Vec2 correction = normal * correctionMag;
    A.body->position -= correction * invMassA;
    B.body->position += correction * invMassB;
}


void PhysicsWorld::resolveCircleVsBox(
    PhysicsObject& circleObj,
    PhysicsObject& boxObj)
{
    auto* cC = static_cast<CircleCollider*>(circleObj.collider);
    auto* cB = static_cast<BoxCollider*>(boxObj.collider);

    Vec2 cPos = circleObj.body->position;
    Vec2 bPos = boxObj.body->position;

    float left   = bPos.x - cB->halfWidth;
    float right  = bPos.x + cB->halfWidth;
    float top    = bPos.y - cB->halfHeight;
    float bottom = bPos.y + cB->halfHeight;

    float closestX = clamp(cPos.x, left, right);
    float closestY = clamp(cPos.y, top, bottom);

    Vec2 closest{ closestX, closestY };
    Vec2 delta = cPos - closest;
    float dist = delta.magnitude();

    Vec2 normal;
    float penetration;

    if (dist > 1e-6f) {
        normal = delta / dist;
        penetration = cC->radius - dist;
    } else {
        float dxL = cPos.x - left;
        float dxR = right - cPos.x;
        float dyT = cPos.y - top;
        float dyB = bottom - cPos.y;

        float minX = std::min(dxL, dxR);
        float minY = std::min(dyT, dyB);

        if (minX < minY) {
            normal = (dxL < dxR) ? Vec2{-1,0} : Vec2{1,0};
            penetration = cC->radius + minX;
        } else {
            normal = (dyT < dyB) ? Vec2{0,-1} : Vec2{0,1};
            penetration = cC->radius + minY;
        }
    }

    if (penetration <= 0.f) return;

    Vec2 contactPoint = closest;

    float invMassC = circleObj.body->invMass;
    float invMassB = boxObj.body->invMass;
    float totalInvMass = invMassC + invMassB;
    if (totalInvMass == 0.f) return;

    // ---------- RELATIVE VELOCITY (WITH ROTATION) ----------
    Vec2 rC = contactPoint - circleObj.body->position;
    Vec2 velC = circleObj.body->velocity +
                perp(rC) * circleObj.body->angularVelocity;
    Vec2 velB = boxObj.body->velocity;

    Vec2 rv = velC - velB;
    float vn = rv.dot(normal);

    // ---------- NORMAL IMPULSE ----------
    float j = 0.f;
    if (vn < 0.f) {
        float restitution = std::min(cC->restitution, cB->restitution);
        if (std::abs(vn) < 0.3f) restitution = 0.f;

        j = -(1.f + restitution) * vn / totalInvMass;

        Vec2 impulse = normal * j;
        circleObj.body->velocity += impulse * invMassC;
        boxObj.body->velocity    -= impulse * invMassB;
    }

    // ---------- FRICTION ----------
    Vec2 velC2 = circleObj.body->velocity +
                 perp(rC) * circleObj.body->angularVelocity;
    Vec2 rv2 = velC2 - velB;
    float vn2 = rv2.dot(normal);

    Vec2 tangent = rv2 - normal * vn2;
    float tMag = tangent.magnitude();

    if (tMag > 1e-4f) {
        tangent /= tMag;

        float jt = -rv2.dot(tangent) / totalInvMass;

        float muS = std::sqrt(
            cC->staticFriction*cC->staticFriction +
            cB->staticFriction*cB->staticFriction);
        float muD = std::sqrt(
            cC->dynamicFriction*cC->dynamicFriction +
            cB->dynamicFriction*cB->dynamicFriction);

        Vec2 frictionImpulse;
        if (std::abs(jt) < j * muS)
            frictionImpulse = tangent * jt;
        else
            frictionImpulse = tangent * -j * muD;

        circleObj.body->velocity += frictionImpulse * invMassC;
        boxObj.body->velocity    -= frictionImpulse * invMassB;

        // 🔥 TORQUE FROM FRICTION
        float torque = cross(rC, frictionImpulse);
        circleObj.body->angularVelocity +=
            torque * circleObj.body->invInertia;
    }

    // ---------- POSITION CORRECTION ----------
    float correctionMag =
        std::max(penetration - penetrationSlop, 0.f) /
        totalInvMass * penetrationPercent;

    Vec2 correction = normal * correctionMag;
    circleObj.body->position += correction * invMassC;
    boxObj.body->position    -= correction * invMassB;
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
    if (totalInvMass == 0.f) return;

    // ---------- NORMAL IMPULSE ----------
    Vec2 rv = B.body->velocity - A.body->velocity;
    float vn = rv.dot(normal);

    float j = 0.f;
    if (vn < 0.f) {
        float restitution = std::min(cA->restitution, cB->restitution);
        if (std::abs(vn) < 0.3f) restitution = 0.f;

        j = -(1.f + restitution) * vn / totalInvMass;

        Vec2 impulse = normal * j;
        A.body->velocity -= impulse * invMassA;
        B.body->velocity += impulse * invMassB;
    }

    // ---------- FRICTION ----------
    rv = B.body->velocity - A.body->velocity;
    float vn2 = rv.dot(normal);

    Vec2 tangent = rv - normal * vn2;
    float tMag = tangent.magnitude();

    const float TANGENT_EPS = 1e-4f;
    if (tMag > TANGENT_EPS) {
        tangent /= tMag;

        float jt = -rv.dot(tangent) / totalInvMass;

        float muS = std::sqrt(
            cA->staticFriction * cA->staticFriction +
            cB->staticFriction * cB->staticFriction);

        float muD = std::sqrt(
            cA->dynamicFriction * cA->dynamicFriction +
            cB->dynamicFriction * cB->dynamicFriction);

        Vec2 frictionImpulse;
        if (std::abs(jt) < j * muS)
            frictionImpulse = tangent * jt;
        else
            frictionImpulse = tangent * -j * muD;

        A.body->velocity -= frictionImpulse * invMassA;
        B.body->velocity += frictionImpulse * invMassB;
    }

    // ---------- POSITION CORRECTION (LAST) ----------
    float correctionMag =
        std::max(penetration - penetrationSlop, 0.f) /
        totalInvMass * penetrationPercent;

    Vec2 correction = normal * correctionMag;
    A.body->position -= correction * invMassA;
    B.body->position += correction * invMassB;
}
