#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <random>
#include <iostream>

#include "math/Vec2.h"
#include "math/math_utils.h"
#include "physics/physicsWorld.h"
#include "deque"
#include "game/objects.h"
#include "test/tests.h"

// ------------------ Scene ------------------

void restingStabilityTest() {

    const float WIDTH  = 800.f;
    const float HEIGHT = 600.f;
    const float WALL_THICKNESS = 20.f;
    const float RADIUS = 40.f;
    const float FIXED_DT = 1.f / 60.f;
    static float accumulator = 0.f;
    std::deque<Rectangle> rectangles;


    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u{800, 600}),
        "SFML Resting Stability Test"
    );
    window.setFramerateLimit(144);

    PhysicsWorld world;
    world.gravity = {0.f, 800.f};


    
    addRectangle(
        world,
        rectangles,
        {400.f, 160.f},
        300.f,
        20.f,
        1.f,
        sf::Color::White
    );
    addRectangle(
        world,
        rectangles,
        {300.f, 260.f},
        100.f,
        60.f,
        1.f,
        sf::Color::Blue
    );
    addRectangle(
        world,
        rectangles,
        {500.f, 260.f},
        100.f,
        60.f,
        1.f,
        sf::Color::Blue
    );
    addRectangle(
        world,
        rectangles,
        {400.f, 360.f},
        300.f,
        20.f,
        1.f,
        sf::Color::Red
    );
    addRectangle(
        world,
        rectangles,
        {400.f, 420.f},
        100.f,
        60.f,
        1.f,
        sf::Color::Green
    );

    // Floor
    addRectangle(
        world,
        rectangles,
        {WIDTH / 2, HEIGHT + WALL_THICKNESS / 2},
        WIDTH, WALL_THICKNESS,
        0.f
    );
    // Ceiling
    addRectangle(
        world,
        rectangles,
        {WIDTH / 2, -WALL_THICKNESS / 2},
        WIDTH, WALL_THICKNESS,
        0.f
    );
    // Left Wall
    addRectangle(
        world,
        rectangles,
        {-WALL_THICKNESS / 2, HEIGHT / 2},
        WALL_THICKNESS, HEIGHT,
        0.f
    );
    // Right Wall
    addRectangle(
        world,
        rectangles,
        {WIDTH + WALL_THICKNESS / 2, HEIGHT / 2},
        WALL_THICKNESS, HEIGHT,
        0.f
    );

    sf::Clock clock;

    // ------------------ Main loop ------------------

    while (window.isOpen()) {
        // ---------- Events ----------
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
        // ---------- Delta time ----------
        accumulator += clock.restart().asSeconds();

        while (accumulator >= FIXED_DT) {
            world.step(FIXED_DT);
            accumulator -= FIXED_DT;
        }

        // ---------- Update ----------
        for (auto& rect : rectangles) {
            rect.shape.setPosition({
                rect.body.position.x,
                rect.body.position.y
            });
        }

        

        // ---------- Render ----------
        window.clear();
        for (auto& rect : rectangles)
            window.draw(rect.shape);
        window.display();
    }
}
