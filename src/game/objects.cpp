#include "game/objects.h"

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

void addRectangle(
    PhysicsWorld& world,
    std::deque<Rectangle>& rectangles,
    Vec2 position,
    float width,
    float height,
    float mass
) {
    rectangles.emplace_back(
        Rectangle{
            RigidBody(position, mass),
            BoxCollider{width/2, height/2},
            sf::RectangleShape({width, height})
        }
    );

    Rectangle& rectangle = rectangles.back();

    rectangle.shape.setOrigin({width/2, height/2});
    rectangle.shape.setFillColor(sf::Color::White);

    world.add(&rectangle.body, &rectangle.collider);
}
