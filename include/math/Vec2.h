#pragma once
#include <cmath>

struct Vec2 {
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float x_, float y_) : x(x_), y(y_) {}

    Vec2 operator+(const Vec2& other) const {
        return { x + other.x, y + other.y };
    }
    Vec2 operator-(const Vec2& other) const {
        return { x - other.x, y - other.y };
    }
    Vec2 operator*(float scalar) const {
        return { x * scalar, y * scalar };
    }
    Vec2 operator/(float scalar) const {
        return { x / scalar, y / scalar };
    }

    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    Vec2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    Vec2& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    float magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    Vec2 normalized() const {
        float len = magnitude();
        if (len == 0) return {0, 0};
        return { x / len, y / len };
    }

    float dot(const Vec2& other) const {
        return x * other.x + y * other.y;
    }

    Vec2 rotated(float angle) const {
        float c = std::cos(angle);
        float s = std::sin(angle);
        return {
            x * c - y * s,
            x * s + y * c
        };
    }
};
