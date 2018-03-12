
// $CXX -std=c++11 main.cpp && ./a.out /path/to/file

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <unordered_map>

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;

const size_t HEADER_SIZE = 80;
const size_t COLOR_SIZE = 2;

template<class T> T read(std::istream& is) {
    char buffer[4];
    is.read(buffer, 4);
    return *(T*)buffer;
}

class Vector3D {
public:
    float x, y, z;
};

class Face {
public:
    Vector3D normal, A, B, C;
};

class StlFile {
public:
    std::string header;
    unsigned int faceCount;
    std::vector<Face> faces;
};

class SmartFace {
public:
    Vector3D normal;
    int A, B, C;
};

class SmartStlFile {
public:
    std::string header;
    unsigned int faceCount;
    std::vector<Vector3D> vertices;
    //std::unordered_map<Vector3D, int> indexMap;
    std::vector<SmartFace> faces;
};

std::string readHeader(std::istream& is) {
    char header[HEADER_SIZE];
    is.read(header, HEADER_SIZE);
    return std::string(header);
}

Vector3D readVector3D(std::istream& is) {
    Vector3D v;

    v.x = read<float>(is);
    v.y = read<float>(is);
    v.z = read<float>(is);

    return v;
}

StlFile basicRead(std::istream& is) {

    StlFile stl;

    stl.header = readHeader(is);
    stl.faceCount = read<unsigned int>(is);
    stl.faces.reserve(stl.faceCount);

    char color_buffer[COLOR_SIZE];
    for (unsigned int i = 0; i < stl.faceCount; ++i) {
        Face face;
        face.normal = readVector3D(is);
        face.A = readVector3D(is);
        face.B = readVector3D(is);
        face.C = readVector3D(is);
        stl.faces.push_back(face);
        is.read(color_buffer, COLOR_SIZE);
    }

    return stl;
}

int main(int argc, char** argv) {

    if (argc < 2) {
        std::cerr << "program requires at least one argument of filename\n";
        return 1;
    }

    std::ifstream file;
    file.open(argv[1], std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Unable to open " << argv[1] << std::endl;
        return 1;
    }

    auto start = steady_clock::now();
    StlFile stl = basicRead(file);
    std::cout << "Basic read time: " << duration_cast<milliseconds>(steady_clock::now() - start).count() << "ms\n";

    return 0;
}
