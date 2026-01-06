#pragma once

#include <SFML/Graphics.hpp>
#include <deque>

#include "math/Vec2.h"
#include "physics/physicsWorld.h"

struct Ball {
    RigidBody body;
    CircleCollider collider;
    sf::CircleShape shape;
};

void addBall(
    PhysicsWorld& world,
    std::deque<Ball>& balls,
    Vec2 position,
    float radius,
    float mass,
    const sf::Color& color = sf::Color::Transparent,
    Vec2 initalVelocity = {0.f, 0.f},
    float restitution = 0.5f
);

struct Rectangle {
    RigidBody body;
    BoxCollider collider;
    sf::RectangleShape shape;
};

void addRectangle(
    PhysicsWorld& world,
    std::deque<Rectangle>& rectangles,
    Vec2 position,
    float width,
    float height,
    float mass,
    const sf::Color& color = sf::Color::Transparent,
    Vec2 initialVelocity = {0.f, 0.f},
    float restitution = 0.5f
);