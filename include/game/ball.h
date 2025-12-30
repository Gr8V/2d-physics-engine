#pragma once

#include <SFML/Graphics.hpp>
#include <deque>

#include <Vec2.h>
#include <physicsWorld.h>

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
