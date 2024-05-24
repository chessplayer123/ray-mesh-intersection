#pragma once

#include <vector>
#include <cstddef>
#include <numeric>
#include <limits>
#include "aabbox.hpp"


template<typename mesh_type, typename float_type, typename index_type>
class Mesh {
public:
    using mesh_t = mesh_type;
    using float_t = float_type;
    using index_t = index_type;

    struct Element {
        index_t index;
        Vector3<float_t> center;
        const mesh_t* mesh;

        template<index_t vertex_num>
        inline Vector3<float_t> v() const
        { return mesh->template v<vertex_num>(index); }
    };

    using iterator = typename std::vector<Element>::iterator;

    constexpr Mesh(index_t size): is_setup(false), elements(size) {
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
        for (index_t i = 0; i < elements.size(); ++i) {
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
