#pragma once
#include <cmath>

struct Vec2 {
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float x_, float y_) : x(x_), y(y_) {}

    // Addition
    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    // Subtraction
    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }

    // Scalar multiply
    Vec2 operator*(float scalar) const {
        return Vec2(x * scalar, y * scalar);
    }

    //magnitude (length)
    float magnitude() const {
        return sqrt(x * x + y * y);
    }

    // Normalize
    Vec2 normalized() const {
        float len = magnitude();
        if (len == 0) return Vec2(0, 0);
        return Vec2(x / len, y / len);
    }

    // Dot product
    float dot(const Vec2& other) const {
        return x * other.x + y * other.y;
    }
};