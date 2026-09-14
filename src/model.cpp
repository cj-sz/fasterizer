#include "model.h"
#include <fstream>
#include <sstream>

int Model::nverts() const {
    return verts.size();
}

int Model::nfaces() const {
    return face_vrts.size();
}

vec3 Model::vert(int i) const {
    return verts.at(i);
}

vec3 Model::vert(int iface, int nthvert) const {
    return verts.at(face_vrts.at(iface).at(nthvert));
}