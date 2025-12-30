#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <random>
#include <iostream>

#include <Vec2.h>
#include <math_utils.h>
#include <physicsWorld.h>
#include <deque>
#include <ball.h>

// ------------------ Scene ------------------

void render_scene() {

    const float WIDTH  = 800.f;
    const float HEIGHT = 600.f;
    const float WALL_THICKNESS = 20.f;
    const float RADIUS = 40.f;
    const float FIXED_DT = 1.f / 60.f;
    static float accumulator = 0.f;
    std::deque<Ball> balls;


    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u{800, 600}),
        "SFML Multi-Ball Demo"
    );
    window.setFramerateLimit(144);

    PhysicsWorld world;
    world.gravity = {0.f, 800.f};
    
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

            if (event->is<sf::Event::MouseButtonPressed>()) {

                const auto* mouseEvent =
                    event->getIf<sf::Event::MouseButtonPressed>();

                if (mouseEvent->button == sf::Mouse::Button::Left) {

                    auto mousePos = sf::Mouse::getPosition(window);
                    
                    addBall(
                        world,
                        balls,
                        {static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)},
                        20.f,
                        1.f
                    );
                }
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

        

        // ---------- Render ----------
        window.clear();
        for (auto& ball : balls)
            window.draw(ball.shape);

        window.draw(RectShape);
        window.display();
    }
}
