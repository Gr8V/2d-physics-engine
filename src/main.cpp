#include <iostream>
#include <vector>
#include <math.h>
#include <Vec2.h>
#include "render_sfml/render.cpp"


int main() {
    Vec2 position(5,6);
    Vec2 velocity(2,3);
    std::cout << "X = " << position.x << '\n';
    std::cout << "Y = " << position.y << '\n';
    std::cout << "Dot Product = " << position.dot(velocity) << '\n';
    std::cout << "Addition = (" << (position+velocity).x << ',' << (position+velocity).y << ")\n";
    render_scene();
    return 0;
}