#pragma once

#include <string>
#include "meshes/triangular_mesh.hpp"

class MeshReader {
public:
    enum DataFormat {
        Ply,
        Stl,
        Obj,
        Auto,
    };

    template<DataFormat>
    static TriangularMesh read_triangular_mesh(const std::string& path);

    MeshReader(std::istream& stream): stream(stream) {}

    inline void expect_line(const std::string& expected_line) {
        std::string line;
        std::getline(stream, line);
        if (expected_line != line) {
            throw "Expected '" + expected_line + "'. But found '" + line + "'.";
        }
    }

    inline void skip_line() {
        std::string line;
        std::getline(stream, line);
    }

    inline void skip_word() {
        read<std::string>();
    }

    inline void expect_word(const std::string& expected_word) {
        std::string actual_word = read<std::string>();
        if (actual_word != expected_word) {
            throw "Expected '" + expected_word + "'. But found '" + actual_word + "'.";
        }
    }

    template <typename T>
    inline T read() {
        T str;
        stream >> str >> std::ws;
        return str;
    }
private:
    std::istream& stream;
};
