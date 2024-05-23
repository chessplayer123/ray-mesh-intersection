#pragma once

#include <vector>
#include <cstddef>
#include <numeric>
#include <limits>
#include "aabbox.hpp"


template<typename mesh_t>
class Mesh {
public:
    struct Element {
        size_t index;
        Vector center;
        const mesh_t* mesh;

        template<size_t vertex_num>
        inline Vector v() const
        { return mesh->template v<vertex_num>(index); }
    };

    using iterator = typename std::vector<Element>::iterator;

    constexpr Mesh(size_t size): is_setup(false), elements(size) {
    }

    inline iterator begin() {
        if (!is_setup) setup();
        return elements.begin();
    }

    inline iterator end() {
        if (!is_setup) setup();
        return elements.end();
    }
private:
    void setup() {
        is_setup = true;
        auto child = static_cast<const mesh_t*>(this);
        for (size_t i = 0; i < elements.size(); ++i) {
            elements[i] = {
                i,
                (child->template v<0>(i) + child->template v<1>(i) + child->template v<2>(i)) / 3,
                child
            };
        }
    }

    bool is_setup;
    std::vector<Element> elements;
};


class TriangularMesh: public Mesh<TriangularMesh> {
public:
    TriangularMesh(): Mesh<TriangularMesh>(0) {
    }

    TriangularMesh(
        std::vector<double>&& coords,
        std::vector<size_t>&& indices
    ):
        Mesh<TriangularMesh>(indices.size() / 3),
        m_size(indices.size() / 3),
        coords(std::move(coords)),
        indices(std::move(indices))
    {}

    template<size_t vertex_num>
    inline Vector v(size_t triangle_num) const {
        size_t i = 3 * indices[3 * triangle_num + vertex_num];
        return Vector(coords[i + 0], coords[i + 1], coords[i + 2]);
    }

    size_t size() const {
        return m_size;
    }

    double* array_buffer() {
        return coords.data();
    }

    size_t* element_array_buffer() {
        return indices.data();
    }
private:
    size_t m_size;
    std::vector<double> coords;
    std::vector<size_t> indices;
};
