#include "math/math_utils.h"

Vec2 reflect(const Vec2& v, const Vec2& normal) {
        return v - normal * (2.f * v.dot(normal));
}

float clamp(float v, float min, float max) {
        if (v < min) return min;
        if (v > max) return max;
        return v;
}
