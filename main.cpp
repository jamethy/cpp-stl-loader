
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

    bool operator==(const Vector3D& other) const {
        return other.x == x && other.y == y && other.z == z;
    }
};

namespace std {
 template <> struct hash<Vector3D> {
    std::size_t operator()(const Vector3D& v) const {
      return ((hash<float>()(v.x) ^ (hash<float>()(v.y) << 1)) >> 1) ^ (hash<float>()(v.z) << 1);
    }
  };
}

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
    std::unordered_map<Vector3D, int> indexMap;
    std::vector<SmartFace> faces;

    int getIndexOf(Vector3D v) {
        if (indexMap.count(v)) {
            return indexMap.at(v);
        } else {
            int index = vertices.size();
            indexMap.insert({v, index});
            vertices.push_back(v);
            return index;
        }
    }
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

StlFile basicRead(char* name) {

    StlFile stl;

    std::ifstream file;
    file.open(name, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Unable to open " << name << std::endl;
        return stl;
    }

    stl.header = readHeader(file);
    stl.faceCount = read<unsigned int>(file);
    stl.faces.reserve(stl.faceCount);

    char color_buffer[COLOR_SIZE];
    for (unsigned int i = 0; i < stl.faceCount; ++i) {
        Face face;
        face.normal = readVector3D(file);
        face.A = readVector3D(file);
        face.B = readVector3D(file);
        face.C = readVector3D(file);
        stl.faces.push_back(face);
        file.read(color_buffer, COLOR_SIZE);
    }

    file.close();
    return stl;
}

SmartStlFile smartRead(char* name) {

    SmartStlFile stl;

    std::ifstream file;
    file.open(name, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Unable to open " << name << std::endl;
        return stl;
    }

    stl.header = readHeader(file);
    stl.faceCount = read<unsigned int>(file);
    stl.faces.reserve(stl.faceCount);
    stl.vertices.reserve(stl.faceCount);
    stl.indexMap.reserve(stl.faceCount);

    char color_buffer[COLOR_SIZE];
    for (unsigned int i = 0; i < stl.faceCount; ++i) {
        SmartFace face;
        face.normal = readVector3D(file);
        face.A = stl.getIndexOf(readVector3D(file));
        face.B = stl.getIndexOf(readVector3D(file));
        face.C = stl.getIndexOf(readVector3D(file));
        stl.faces.push_back(face);
        file.read(color_buffer, COLOR_SIZE);
    }

    file.close();
    return stl;
}

int main(int argc, char** argv) {

    if (argc < 2) {
        std::cerr << "program requires at least one argument of filename\n";
        return 1;
    }

    auto start = steady_clock::now();
    StlFile stl = basicRead(argv[1]);
    std::cout << "Basic read time: " << duration_cast<milliseconds>(steady_clock::now() - start).count() << "ms\n";
    std::cout << "FaceCount: " << stl.faceCount << std::endl << std::endl;

    start = steady_clock::now();
    SmartStlFile smartStl = smartRead(argv[1]);
    std::cout << "Smart read time: " << duration_cast<milliseconds>(steady_clock::now() - start).count() << "ms\n";
    std::cout << "FaceCount: " << smartStl.faceCount << std::endl;
    std::cout << "Vertex Count: " << smartStl.vertices.size() << std::endl << std::endl;

    return 0;
}
