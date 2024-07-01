#pragma once

#include "rmi.hpp"

template<typename _float_t, typename _index_t>
class RawMesh: public rmi::Mesh<RawMesh<_float_t, _index_t>> {
public:
    using float_t = _float_t;
    using index_t = _index_t;

    RawMesh(
        std::vector<float_t>&& coords,
        std::vector<index_t>&& indices
    ):
        m_size(indices.size() / 3),
        m_vertices(std::move(coords)),
        m_indices(std::move(indices))
    {
        rmi::Mesh<RawMesh>::setup(size());
    }

    // the only required method
    template<index_t vertex_num>
    inline rmi::Vector3<float_t> v(index_t triangle_num) {
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
    size_t m_size;
    std::vector<float_t> m_vertices;
    std::vector<index_t> m_indices;
};

typedef RawMesh<double, size_t> TriangularMesh;

typedef RawMesh<float, unsigned int> WebGLMesh;
