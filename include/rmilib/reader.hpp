#pragma once

#include <string>
#include <memory>
#include "triangular_mesh.hpp"

class MeshReader {
public:
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

    inline void skip_bytes(std::streamsize size) {
        read_as_bytes(size);
    }

    inline void expect_word(const std::string& expected_word) {
        std::string actual_word = read<std::string>();
        if (actual_word != expected_word) {
            throw "Expected '" + expected_word + "'. But found '" + actual_word + "'.";
        }
    }

    template <typename T>
    inline T read_as_bytes() {
        auto buffer = std::shared_ptr<char[]>(new char[sizeof(T)]);
        stream.read(buffer.get(), sizeof(T));
        auto value = std::shared_ptr<T>(buffer, reinterpret_cast<T*>(buffer.get()));
        return *value;
    }

    inline std::string read_as_bytes(std::streamsize size) {
        auto buffer = std::shared_ptr<char[]>(new char[size+1]);
        buffer.get()[size] = '\0';

        stream.read(buffer.get(), size);
        return std::string(buffer.get());
    }

    template <typename T>
    inline T read() {
        T data;
        stream >> data >> std::ws;
        return data;
    }
private:
    std::istream& stream;
};

enum DataFormat {
    Ply,
    Stl,
    Obj
};

inline DataFormat define_format(const std::string& filename) {
    size_t index = filename.find_last_of('.');
    if (index == std::string::npos) {
        throw "Can't define file format";
    }
    std::string extension = filename.substr(index + 1);

    if (extension == "ply") {
        return Ply;
    } else if (extension == "stl") {
        return Stl;
    } else if (extension == "obj") {
        return Obj;
    } else {
        throw "Unsupported file format";
    }
}

template<DataFormat>
TriangularMesh read_triangular_mesh(std::istream& stream);

TriangularMesh read_triangular_mesh(const std::string& path);

