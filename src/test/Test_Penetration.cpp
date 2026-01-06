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

void penetrationTest() {

    const float WIDTH  = 800.f;
    const float HEIGHT = 600.f;
    const float WALL_THICKNESS = 20.f;
    const float RADIUS = 40.f;
    const float FIXED_DT = 1.f / 60.f;
    static float accumulator = 0.f;
    std::deque<Ball> balls;
    std::deque<Rectangle> rectangles;


    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u{800, 600}),
        "SFML Penetration Test"
    );
    window.setFramerateLimit(144);

    PhysicsWorld world;
    world.gravity = {0.f, 800.f};

    // All Rectangles vertically overlap by 10 pixels
    addRectangle(
        world,
        rectangles,
        {400.f, 550.f},
        60.f, 60.f,
        1.f,
        sf::Color::Red
    );
    addRectangle(
        world,
        rectangles,
        {430.f, 500.f},
        60.f, 60.f,
        1.f,
        sf::Color::Red
    );
    addRectangle(
        world,
        rectangles,
        {460.f, 450.f},
        60.f, 60.f,
        1.f,
        sf::Color::Red
    );
    addRectangle(
        world,
        rectangles,
        {490.f, 400.f},
        60.f, 60.f,
        1.f,
        sf::Color::Red
    );
    addRectangle(
        world,
        rectangles,
        {520.f, 350.f},
        60.f, 60.f,
        1.f,
        sf::Color::Red
    );

    // Floor
    addRectangle(
        world,
        rectangles,
        {WIDTH / 2, HEIGHT + WALL_THICKNESS / 2},
        WIDTH, WALL_THICKNESS,
        0.f,
        sf::Color::White,
        {},
        1.f
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
        for (auto& ball : balls) {
            ball.shape.setPosition({
                ball.body.position.x,
                ball.body.position.y
            });
        }
        for (auto& rect : rectangles) {
            rect.shape.setPosition({
                rect.body.position.x,
                rect.body.position.y
            });
        }

        

        // ---------- Render ----------
        window.clear();
        for (auto& ball : balls)
            window.draw(ball.shape);
        for (auto& rect : rectangles)
            window.draw(rect.shape);
        window.display();
    }
}
