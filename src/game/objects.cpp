#include "game/objects.h"

void addBall(
    PhysicsWorld& world,
    std::deque<Ball>& balls,
    Vec2 position,
    float radius,
    float mass,
    const sf::Color& color,
    Vec2 initalVelocity,
    float restitution
) {
    balls.emplace_back(
        Ball{
            RigidBody(position, mass),
            CircleCollider{radius},
            sf::CircleShape(radius)
        }
    );

    Ball& ball = balls.back();

    ball.body.velocity = initalVelocity;
    ball.collider.restitution = restitution;

    ball.shape.setOrigin({radius, radius});
    ball.shape.setFillColor(color);

    world.add(&ball.body, &ball.collider);
}

void addRectangle(
    PhysicsWorld& world,
    std::deque<Rectangle>& rectangles,
    Vec2 position,
    float width,
    float height,
    float mass,
    const sf::Color& color,
    Vec2 initalVelocity,
    float restitution
) {
    rectangles.emplace_back(
        Rectangle{
            RigidBody(position, mass),
            BoxCollider{width/2, height/2},
            sf::RectangleShape({width, height})
        }
    );

    Rectangle& rectangle = rectangles.back();

    rectangle.body.velocity = initalVelocity;
    rectangle.collider.restitution = restitution;

    rectangle.shape.setOrigin({width/2, height/2});
    rectangle.shape.setFillColor(color);

    world.add(&rectangle.body, &rectangle.collider);
}
