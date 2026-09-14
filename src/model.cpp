#include "model.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

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

bool Model::load(const std::string &path) {
    // this declares "in" as the input file stream; below line opens file
    std::ifstream in(path);
    // false if could not be opened (ifstream overloads the boolean operator)
    if (!in){
        throw std::runtime_error("Model::load: failed to open file: " + path);
    }

    // define a string for each line of the file
    std::string line;
    // getline reads chars from in up to \n and stores them in line; discards \n 
    // it returns in so while performs the same overload check on if in still has
    // lines left to read
    while (std::getline(in, line)) {
        // define another stream that looks at line specifically, 
        std::istringstream iss(line);
        std::string token;
        // iss reads whitespace-delimited chunks of the stream as it was defined
        iss >> token;

        if (token == "v") {
            // chunk 3 reads into the vars and make the vector
            float x, y, z;
            iss >> x >> y >> z;
            verts.push_back(vec3{x, y, z});
        } else if (token == "f") {
            std::array<int, 3> f;
            for (int i = 0; i < 3; i ++) {
                std::string chunk;
                iss >> chunk;
                std::istringstream cs(chunk);
                std::string idx_str;
                // getline delimited by passe in character
                std::getline(cs, idx_str, '/');
                // writes the first number to the ith slot
                // the first number is the vertex slot
                f[i] = std::stoi(idx_str) - 1;
            }
            face_vrts.push_back(f);
        }
    }

    return true;
}