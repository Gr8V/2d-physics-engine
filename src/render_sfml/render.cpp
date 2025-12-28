#include <SFML/Graphics.hpp>

void render_scene(){
    // Create a window
    sf::RenderWindow window(
        sf::VideoMode(800, 600),
        "SFML Demo Scene"
    );
    window.setFramerateLimit(60);

    // Create a circle
    sf::CircleShape ball(40.f);
    ball.setFillColor(sf::Color::Cyan);
    ball.setPosition(100.f, 300.f);

    // Velocity (for animation)
    sf::Vector2f velocity(200.f, 0.f);

    sf::Clock clock;

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Delta time
        float dt = clock.restart().asSeconds();

        // Move the ball
        ball.move(velocity * dt);

        // Bounce off walls
        if (ball.getPosition().x < 0 ||
            ball.getPosition().x + ball.getRadius() * 2 > 800) {
            velocity.x *= -1;
        }

        // Render
        window.clear(sf::Color(20, 20, 25)); // Dark background
        window.draw(ball);
        window.display();
    }
}