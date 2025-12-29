#include <SFML/Graphics.hpp>
#include <Vec2.h>
#include <random>
#include <iostream>

sf::Vector2f toSFML(const Vec2& v) {
    return { v.x, v.y };
}


float randomFloat(float min, float max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}
Vec2 reflect(const Vec2& v, const Vec2& normal) {
        return v - normal * (2.f * v.dot(normal));
        }
        Vec2 rotate(const Vec2& v, float angle) {
        float c = std::cos(angle);
        float s = std::sin(angle);
        return {
            v.x * c - v.y * s,
            v.x * s + v.y * c
        };
    }


void render_scene(){
    // Create a window (SFML 3 requires Vector2u)
    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u{800, 600}),
        "SFML Demo Scene"
    );
    window.setFramerateLimit(144);

    // Create a circle
    sf::CircleShape ball(40.f);
    ball.setFillColor(sf::Color::Red);
    Vec2 position{100.0f,300.0f};
    ball.setPosition(toSFML(position));

    // Velocity
    Vec2 velocity{600.f, 360.f};

    sf::Clock clock;

    // Main loop
    while (window.isOpen()) {

        // ---- Event handling (SFML 3 style) ----
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // ---- Delta time ----
        float dt = clock.restart().asSeconds();

        // ---- Update ----
        position += velocity * dt;

        // Bounce off walls
        const float radius = ball.getRadius();
        const float diameter = radius * 2.f;

        const float width = 800.f;
        const float height = 600.f;

        bool bounced = false;
        Vec2 normal;

        if (position.x < 0.f) {
            position.x = 0.f;
            normal = { 1.f, 0.f };
            bounced = true;
        }

        if (position.x + diameter > width) {
            position.x = width - diameter;
            normal = { -1.f, 0.f };
            bounced = true;
        }

        if (position.y < 0.f) {
            position.y = 0.f;
            normal = { 0.f, 1.f };
            bounced = true;
        }

        if (position.y + diameter > height) {
            position.y = height - diameter;
            normal = { 0.f, -1.f };
            bounced = true;
        }

        if (bounced) {
            std::cout << "Ball Bounced\n";
            velocity = reflect(velocity, normal);

            // Add slight randomness (±10 degrees)
            float angle = randomFloat(-0.17f, 0.17f);
            velocity = rotate(velocity, angle);
        }

        ball.setPosition(toSFML(position));


        // ---- Render ----
        window.clear(sf::Color{20, 20, 25});
        window.draw(ball);
        window.display();
    }
}