![build status](https://github.com/chessplayer123/ray-mesh-intersection/actions/workflows/cmake-multi-platform.yml/badge.svg)
# Ray And Triangular Mesh Intersection Library


## Usage

### Create wrapper for mesh [rmi.hpp](include/rmilib/rmi.hpp)
```cpp
#include "rmi.hpp"

class MyWrapperClassName: public rmi::Mesh<MyWrapperClassName> {
public:
    using float_t = my_float_t;
    using index_t = my_index_t;

    MyWrapperClassName(...)
    {
        ...
        rmi::Mesh<MyWrapperClassName>::setup(size);
    }

    template<index_t vertex_num>
    inline rmi::Vector3<float_t> v(index_t triangle_index) const {
        ...
        return rmi::Vector3<float_t>(x, y, z);
    }
};
```

### Build tree and find intersections
```cpp
const MyWrapperClassName mesh(...);

auto splitter = rmi::MedianSplitter<MyWrapperClassName>();  // rmi::SAHSplitter<MyWrapperClassName>()

const auto tree = rmi::KDTree<MyWrapperClassName>::for_mesh(mesh, splitter);  // SAH by default

const rmi::Ray<my_float_t> ray(
    rmi::Vector3<my_float_t>(...), // origin
    rmi::Vector3<my_float_t>(...)  // direction
);

std::vector<rmi::Vector3<my_float_t>> points = ray.intersects(tree);

std::vector<rmi::Vector3<my_float_t>> points = ray.intersects(mesh);
```

### Or use parallel algorithms (pool requires [external/wsq.hpp](external/wsq.hpp))
```cpp
#define RMI_INCLUDE_OMP
#define RMI_INCLUDE_POOL
#include "rmi.hpp"

const int threads_count = 4;

const auto tree = rmi::KDTree<MyWrapperClassName>::omp_for_mesh(mesh, threads_count, splitter);

std::vector<rmi::Vector3<my_float_t>> points = ray.omp_intersects(tree, threads_count);

std::vector<rmi::Vector3<my_float_t>> points = ray.omp_intersects(mesh, threads_count);

std::vector<rmi::Vector3<my_float_t>> points = ray.pool_intersects(tree, threads_count);
```

## Build
```
cmake -S . -B build [-DBUILD_TESTS=ON] [-DINCLUDE_OMP=ON] [-DINCLUDE_POOL=ON]
cd build
make -j%
```
### WASM (emscripten < 3.1.61)
```
emcmake cmake -S . -B build -DBUILD_WASM=ON -DBUILD_OMP=ON
cd build
make -j%
```

## Tests
```
ctest -j%
ctest -j% -L benchmark
ctest -j% -L unittest
```
