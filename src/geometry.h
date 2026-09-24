#pragma once
#include <cmath>
#include <optional>
#include <utility>
#include <algorithm>

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

    float length() const {
        return std::sqrt(dot(*this));
    }

    vec2 normalize() const {
        return *this * (1.0f / length());
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

    float length() const {
        return std::sqrt(dot(*this));
    }

    vec3 normalize() const {
        return *this * (1.0f / length());
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

template<int N> struct mat {
    float m[N][N] = {}; //initialize all floats to 0 so no garbage

    // mutable and immutable versions of quick-indexing
    // no bounds checking so other code is responsible for this
    float *operator[](int i) { return m[i]; }
    const float *operator[](int i) const { return m[i]; }
    
    // static way to get an identity matrix of any size
    static mat<N> identity() {
        mat A; // all values defaulted to zero and n known because of static call
        for (int i = 0; i < N; i++) {
            A[i][i] = 1.0f;
        }
        return A;
    }

    // call transpose T; returns a new matrix
    mat<N> T() const {
        mat R;
        // iterate over rows on outside
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j ++) {
                T[j][i] = R[i][j];
            }
        }
        return R;
    }

    // for inversion, gauss-jordan elimination
    // optional because it is possible the matrix does not have an inverse
    std::optional<mat<N>> inverse() const {
        constexpr float epsilon = 1e-6f;
        mat I = identity();
        mat A = *this;
        // iterate over the columns
        for (int c = 0; c < N; c++) {
            float max = 0.0f;
            int max_row = -1;
            // find the row r with the largest [r][c]
            for (int r = c; r < N; r++) {
                float cur = std::abs(A[r][c]);
                if (cur > max) {
                    max = cur;
                    max_row = r;
                }
            }
            // if this max is zero (with some float tolerance) we have no inverse
            if (max <= epsilon) {
                return std::nullopt;
            }

            // If we did find a suitable max, swap the rows in I and A:
            std::swap(A.m[max_row], A.m[c]);
            std::swap(I.m[max_row], I.m[c]);

            float pivot = A[c][c];

            // Normalize pivot row 
            for (int i = 0; i < N; i++) {
                A[c][i] /= pivot;
                I[c][i] /= pivot;
            }
            
            // Eliminate the column
            for (int r = 0; r < N; r++) {
                if (r == c) continue;
                float f = A[r][c];
                for (int col = 0; col < N; col++) {
                    A[r][col] -= f * A[c][col];
                    I[r][col] -= f * I[c][col];
                }
            }
        }

        return I;
    }
};

// matmul overloads for vec2, vec3, vec4
// inline means we just write the operation and the compiler type-checks
// the left and right sides of it, and uses this if they match 
// and that this function sits outside a struct
// if we implemented these in a .cpp file we would declare a header here
// without the inline modifier, and no variable names
inline vec2 operator*(const mat<2> &A, const vec2 &v) {
    return vec2{A[0][0] * v.x + A[0][1] * v.y, A[1][0] * v.x + A[1][1] * v.y};
}

inline vec3 operator*(const mat<3> &A, const vec3 &v) {
    return vec3{
        A[0][0] * v.x + A[0][1] * v.y + A[0][2] * v.z,
        A[1][0] * v.x + A[1][1] * v.y + A[1][2] * v.z,
        A[2][0] * v.x + A[2][1] * v.y + A[2][2] * v.z
    };
}

inline vec4 operator*(const mat<4> &A, const vec4 &v) {
    return vec4{
        A[0][0] * v.x + A[0][1] * v.y + A[0][2] * v.z + A[0][3] * v.w,
        A[1][0] * v.x + A[1][1] * v.y + A[1][2] * v.z + A[1][3] * v.w,
        A[2][0] * v.x + A[2][1] * v.y + A[2][2] * v.z + A[2][3] * v.w,
        A[3][0] * v.x + A[3][1] * v.y + A[3][2] * v.z + A[3][3] * v.w,
    };
}