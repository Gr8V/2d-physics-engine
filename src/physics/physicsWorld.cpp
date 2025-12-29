#include <physicsWorld.h>


void PhysicsWorld::integrate(float dt)
{
    for (auto& obj : objects) {
        if (obj.body->invMass == 0.f) continue;

        obj.body->applyForce(gravity * obj.body->mass);
        obj.body->integrate(dt);
    }
}

void PhysicsWorld::solveCollisions()
for (size_t i = 0; i < count; i++)
{
    /* code */
}

{
    for (size_t i = 0; i < objects.size(); i++) {
        for (size_t j = i + 1; j < objects.size(); j++) {
            auto& A = objects[i];
            auto& B = objects[j];

            Vec2 delta = B.body->position - A.body->position;
            float dist = delta.magnitude();
            float penetration =
                (A.collider->radius + B.collider->radius) - dist;

            if (penetration > 0.f) {
                Vec2 normal = delta.normalized();

                float totalInvMass =
                    A.body->invMass + B.body->invMass;

                if (totalInvMass == 0.f) continue;

                Vec2 correction =
                    normal * (penetration / totalInvMass);

                A.body->position -= correction * A.body->invMass;
                B.body->position += correction * B.body->invMass;
            }
        }
    }
}
