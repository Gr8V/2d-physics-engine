#pragma once

#include <SFML/Graphics.hpp>
#include <random>
#include "Vec2.h"

inline sf::Vector2f toSFML(const Vec2& v) {
    return { v.x, v.y };
}

inline float randomFloat(float min, float max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}
