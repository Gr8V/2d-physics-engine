#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <iostream>

#include <Vec2.h>
#include <math_utils.h>
#include <physicsWorld.h>

// ------------------ Utilities ------------------

sf::Vector2f toSFML(const Vec2& v) {
    return { v.x, v.y };
}

float randomFloat(float min, float max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

// ------------------ Ball ------------------

struct Ball {
    Vec2 position;
    Vec2 velocity;
    sf::CircleShape shape;
};

// ------------------ Scene ------------------

void render_scene_bounce_test() {

    const float WIDTH  = 800.f;
    const float HEIGHT = 600.f;
    const float RADIUS = 40.f;

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u{800, 600}),
        "SFML Multi-Ball Demo"
    );

    window.setFramerateLimit(144);

    std::vector<Ball> balls;
    sf::Clock clock;

    // ------------------ Main loop ------------------

    while (window.isOpen()) {

        // ---------- Events ----------
        while (const auto event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (event->is<sf::Event::MouseButtonPressed>()) {

                const auto* mouseEvent =
                    event->getIf<sf::Event::MouseButtonPressed>();

                if (mouseEvent->button == sf::Mouse::Button::Left) {

                    auto mousePos = sf::Mouse::getPosition(window);

                    Ball ball;
                    ball.position = {
                        mousePos.x - RADIUS,
                        mousePos.y - RADIUS
                    };

                    ball.velocity = {
                        randomFloat(-600.f, 600.f),
                        randomFloat(-600.f, 600.f)
                    };

                    ball.shape = sf::CircleShape(RADIUS);
                    ball.shape.setFillColor(sf::Color::Red);
                    ball.shape.setPosition(toSFML(ball.position));

                    balls.push_back(ball);
                }
            }
        }

        // ---------- Delta time ----------
        float dt = clock.restart().asSeconds();

        // ---------- Update ----------
        for (auto& ball : balls) {

            ball.position += ball.velocity * dt;

            const float diameter = RADIUS * 2.f;
            Vec2 normal;
            bool bounced = false;

            if (ball.position.x < 0.f) {
                ball.position.x = 0.f;
                normal = { 1.f, 0.f };
                bounced = true;
            }

            if (ball.position.x + diameter > WIDTH) {
                ball.position.x = WIDTH - diameter;
                normal = { -1.f, 0.f };
                bounced = true;
            }

            if (ball.position.y < 0.f) {
                ball.position.y = 0.f;
                normal = { 0.f, 1.f };
                bounced = true;
            }

            if (ball.position.y + diameter > HEIGHT) {
                ball.position.y = HEIGHT - diameter;
                normal = { 0.f, -1.f };
                bounced = true;
            }

            if (bounced) {
                ball.velocity = reflect(ball.velocity, normal);
                ball.velocity = ball.velocity.rotated(
                    randomFloat(-0.17f, 0.17f)
                );
            }

            ball.shape.setPosition(toSFML(ball.position));
        }

        // ---------- Render ----------
        window.clear(sf::Color{20, 20, 25});

        for (auto& ball : balls) {
            window.draw(ball.shape);
        }

        window.display();
    }
}


void render_scene() {

    const float WIDTH  = 800.f;
    const float HEIGHT = 600.f;
    const float RADIUS = 40.f;

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u{800, 600}),
        "SFML Multi-Ball Demo"
    );
    window.setFramerateLimit(144);

    PhysicsWorld world;
    world.gravity = {0.f, 800.f};

    //add a falling ball
    RigidBody ballBody({400.f, 100.f}, 1.f);
    CircleCollider ballCollider{20.f};
    world.add(&ballBody, &ballCollider);
    sf::CircleShape ballShape(ballCollider.radius);
    ballShape.setOrigin({ballCollider.radius, ballCollider.radius});
    ballShape.setFillColor(sf::Color::White);

    //add a floor
    RigidBody floorBody({400.f, 550.f}, 0.f); // mass = 0 → static
    CircleCollider floorCollider{300.f};
    world.add(&floorBody, &floorCollider);
    sf::CircleShape floorShape(floorCollider.radius);
    floorShape.setOrigin({floorCollider.radius, floorCollider.radius});
    floorShape.setPosition({floorBody.position.x, floorBody.position.y});
    floorShape.setFillColor(sf::Color::White);


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
        float dt = clock.restart().asSeconds();

        // ---------- Update ----------
        world.step(dt);
        ballShape.setPosition({
            ballBody.position.x,
            ballBody.position.y
        });

        // ---------- Render ----------
        window.clear();
        window.draw(ballShape);
        window.draw(floorShape);
        window.display();
    }
}
