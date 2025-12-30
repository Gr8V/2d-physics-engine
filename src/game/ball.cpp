#include <Ball.h>

void addBall(
    PhysicsWorld& world,
    std::deque<Ball>& balls,
    Vec2 position,
    float radius,
    float mass
) {
    balls.emplace_back(
        Ball{
            RigidBody(position, mass),
            CircleCollider{radius},
            sf::CircleShape(radius)
        }
    );

    Ball& ball = balls.back();

    ball.shape.setOrigin({radius, radius});
    ball.shape.setFillColor(sf::Color::White);

    world.add(&ball.body, &ball.collider);
}
