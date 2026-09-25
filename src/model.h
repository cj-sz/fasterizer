#pragma once
#include "geometry.h"
#include <cstdint>
#include <string>
#include <vector>
#include <array>

class Model {
    public:
        bool load(const std::string &path);
        int nverts() const;
        int nfaces() const;
        vec3 vert(int i) const;
        // get a faces ith vertex as a vec3
        vec3 vert(int iface, int nthvert) const;

    private:
        std::vector<vec3> verts;
        std::vector<std::array<int,3>> face_vrts;
};