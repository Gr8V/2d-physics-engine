#include "math_utils.h"

Vec2 reflect(const Vec2& v, const Vec2& normal) {
        return v - normal * (2.f * v.dot(normal));
}