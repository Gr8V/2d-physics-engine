#pragma once
#include "math/Vec2.h"

Vec2 reflect(const Vec2& v, const Vec2& normal);
float clamp(float v, float min, float max);
Vec2 perp(const Vec2& v);
float cross(const Vec2& a, const Vec2& b);