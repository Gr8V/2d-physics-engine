#include <SFML/Graphics.hpp>

void render_scene(){
    // Create a window (SFML 3 requires Vector2u)
    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u{800, 600}),
        "SFML Demo Scene"
    );
    window.setFramerateLimit(60);

    // Create a circle
    sf::CircleShape ball(40.f);
    ball.setFillColor(sf::Color::Cyan);
    ball.setPosition({100.f, 300.f});

    // Velocity
    sf::Vector2f velocity{200.f, 0.f};

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
        ball.move(velocity * dt);

        // Bounce off walls
        const float radius = ball.getRadius();
        const float diameter = radius * 2.f;

        if (ball.getPosition().x < 0.f ||
            ball.getPosition().x + diameter > 800.f) {
            velocity.x *= -1.f;
        }

        // ---- Render ----
        window.clear(sf::Color{20, 20, 25});
        window.draw(ball);
        window.display();
    }
}