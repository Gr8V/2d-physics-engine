#pragma once

struct CircleCollider {
    float radius = 0.f;
    float restitution = 0.5f;
};

struct BoxCollider {
    float halfWidth;
    float halfHeight;
    float restitution = 0.5f;
};