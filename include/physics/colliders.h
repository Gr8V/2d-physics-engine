#pragma once

struct CircleCollider {
    float radius = 0.f;
    float restitution = 0.5f;
    float staticFriction  = 0.6f;
    float dynamicFriction = 0.4f;
};

struct BoxCollider {
    float halfWidth;
    float halfHeight;
    float restitution = 0.5f;
    float staticFriction  = 0.6f;
    float dynamicFriction = 0.4f;
};