#pragma once
#include <cmath>

struct vec2 {
    float x, y;

    vec2 operator+(const vec2 &other) const {
        return vec2{x + other.x, y + other.y};
    }

    vec2 operator-(const vec2 &other) const {
        return vec2{x - other.x, y - other.y};
    }

    vec2 operator*(float f) const {
        return vec2{x * f, y * f};
    }

    vec2 operator-() const {
        return vec2{-x, -y};
    }
};

struct vec3 {
    float x, y, z;

    // define some operators on these vectors
    // this function guarantees that the vector passed in will not be edited
    // trailing const says that the left hand operand will not be edited
    vec3 operator+(const vec3 &other) const {
        return vec3{x + other.x, y + other.y, z + other.z};
    }

    vec3 operator-(const vec3 &other) const {
        return vec3{x - other.x, y - other.y, z - other.z};
    }

    vec3 operator*(float f) const {
        return vec3{x * f, y * f, z * f};
    }

    vec3 operator-() const {
        return vec3{-x, -y, -z};
    }

    float min() const {
        return std::min(std::min(x, y), z);
    }
};