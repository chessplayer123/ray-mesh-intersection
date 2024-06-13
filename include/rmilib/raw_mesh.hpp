#pragma once

#include "rmi.h"

template<typename float_t, typename index_t>
class RawMesh: public rmi::Mesh<RawMesh<float_t, index_t>, float_t, index_t> {
public:
    RawMesh(): rmi::Mesh<RawMesh, float_t, index_t>(0) {
    }

    RawMesh(
        std::vector<float_t>&& coords,
        std::vector<index_t>&& indices
    ):
        rmi::Mesh<RawMesh, float_t, index_t>(indices.size() / 3),
        m_size(indices.size() / 3),
        m_vertices(std::move(coords)),
        m_indices(std::move(indices))
    {}

    // the only required method
    template<index_t vertex_num>
    inline rmi::Vector3<float_t> v(index_t triangle_num) const {
        index_t i = 3 * m_indices[3 * triangle_num + vertex_num];
        return rmi::Vector3<float_t>(m_vertices[i + 0], m_vertices[i + 1], m_vertices[i + 2]);
    }

    index_t size() const {
        return m_size;
    }

    const std::vector<float_t>& vertices() const {
        return m_vertices;
    }

    const std::vector<index_t>& indices() const {
        return m_indices;
    }
private:
    index_t m_size;
    std::vector<float_t> m_vertices;
    std::vector<index_t> m_indices;
};

typedef RawMesh<double, size_t> TriangularMesh;

typedef RawMesh<float, unsigned int> WebGLMesh;
