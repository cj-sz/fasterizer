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

    float dot(const vec2 &v) const {
        return x * v.x + y * v.y;
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

    float dot(const vec3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    vec3 cross(const vec3 &b) const {
        float vx = y * b.z - z * b.y;
        float vy = z * b.x - x * b.z;
        float vz = x * b.y - y * b.x;
        return vec3{vx, vy, vz};
    }
};

struct vec4 {
    float x, y, z, w;

    vec4 operator+(const vec4 &other) const {
        return vec4{x + other.x, y + other.y, z + other.z, w + other.w};
    }
    vec4 operator-(const vec4 &other) const {
        return vec4{x - other.x, y - other.y, z - other.z, w - other.w};
    }
    vec4 operator*(float f) const {
        return vec4{x*f, y*f, z*f, w*f};
    }
    vec4 operator-() const {
        return vec4{-x, -y, -z, -w};
    }
    float dot (const vec4 &v) const {
        return x * v.x + y * v.y + z * v.z + w * v.w;
    }
};