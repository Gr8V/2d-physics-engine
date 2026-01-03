cmake_minimum_required(VERSION 3.16)

project(PhysicsEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(physics_engine ${SOURCES})

target_include_directories(physics_engine PRIVATE
    include
)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

target_link_libraries(physics_engine
    SFML::Graphics
    SFML::Window
    SFML::System
)
