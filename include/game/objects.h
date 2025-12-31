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
    float mass
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
    float mass
);