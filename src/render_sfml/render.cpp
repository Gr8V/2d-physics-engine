#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <iostream>

#include <Vec2.h>
#include <math_utils.h>

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

void render_scene() {

    const float WIDTH  = 800.f;
    const float HEIGHT = 600.f;
    const float RADIUS = 40.f;

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u{WIDTH, HEIGHT}),
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
