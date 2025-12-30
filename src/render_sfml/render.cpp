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
    const float WALL_THICKNESS = 20.f;
    const float RADIUS = 40.f;
    const float FIXED_DT = 1.f / 60.f;
    static float accumulator = 0.f;

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u{800, 600}),
        "SFML Multi-Ball Demo"
    );
    window.setFramerateLimit(144);

    PhysicsWorld world;
    world.gravity = {0.f, 800.f};

    //add a falling ball
    RigidBody ballBody({400.f, 150.f}, 1.f);
    CircleCollider ballCollider{20.f};
    world.add(&ballBody, &ballCollider);
    sf::CircleShape ballShape(ballCollider.radius);
    ballShape.setOrigin({ballCollider.radius, ballCollider.radius});
    ballShape.setFillColor(sf::Color::White);

    //add a falling ball
    RigidBody ballBody2({380.f, 50.f}, 1.f);
    CircleCollider ballCollider2{20.f};
    world.add(&ballBody2, &ballCollider2);
    sf::CircleShape ballShape2(ballCollider2.radius);
    ballShape2.setOrigin({ballCollider2.radius, ballCollider2.radius});
    ballShape2.setFillColor(sf::Color::White);

    //add a floor
    RigidBody RectBody({400.f, 500.f}, 0.f);
    BoxCollider RectCollider{400.f, 20.f};
    world.add(&RectBody, &RectCollider);
    sf::RectangleShape RectShape({RectCollider.halfWidth*2, RectCollider.halfHeight*2});
    RectShape.setOrigin({RectCollider.halfWidth, RectCollider.halfHeight});
    RectShape.setPosition({RectBody.position.x, RectBody.position.y});
    RectShape.setFillColor(sf::Color::White);

    RigidBody floorBody({WIDTH / 2, HEIGHT + WALL_THICKNESS / 2}, 0.f);
    BoxCollider floorCollider{WIDTH / 2, WALL_THICKNESS / 2};
    world.add(&floorBody, &floorCollider);

    RigidBody ceilingBody({WIDTH / 2, -WALL_THICKNESS / 2}, 0.f);
    BoxCollider ceilingCollider{WIDTH / 2, WALL_THICKNESS / 2};
    world.add(&ceilingBody, &ceilingCollider);

    RigidBody leftWallBody({-WALL_THICKNESS / 2, HEIGHT / 2}, 0.f);
    BoxCollider leftWallCollider{WALL_THICKNESS / 2, HEIGHT / 2};
    world.add(&leftWallBody, &leftWallCollider);

    RigidBody rightWallBody({WIDTH + WALL_THICKNESS / 2, HEIGHT / 2}, 0.f);
    BoxCollider rightWallCollider{WALL_THICKNESS / 2, HEIGHT / 2};
    world.add(&rightWallBody, &rightWallCollider);




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
        ballShape.setPosition({
            ballBody.position.x,
            ballBody.position.y
        });
        ballShape2.setPosition({
            ballBody2.position.x,
            ballBody2.position.y
        });
        

        // ---------- Render ----------
        window.clear();
        window.draw(ballShape);
        window.draw(ballShape2);
        window.draw(RectShape);
        window.display();
    }
}
