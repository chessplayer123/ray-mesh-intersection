![build status](https://github.com/chessplayer123/ray-mesh-intersection/actions/workflows/cmake-multi-platform.yml/badge.svg)
# Ray And Triangular Mesh Intersection Library


## Usage

### Create wrapper for mesh [rmi.hpp](include/rmilib/rmi.hpp)
```cpp
#include "rmi.hpp"

class MyWrapperClassName: public rmi::Mesh<MyWrapperClassName, my_float_t, my_index_t> {
public:
    MyWrapperClassName(...):
        rmi::Mesh<MyWrapperClassName, my_float_t, my_index_t>(size_of_mesh), ...
    {
       ...
    }

    template<my_index_t vertex_num>
    inline rmi::Vector3<my_float_t> v(my_index_t triangle_index) const {
        ...
        return rmi::Vector3<my_float_t>(x, y, z);
    }
};
```

### Build tree and find intersections
```cpp
const MyWrapperClassName mesh(...);
const int depth = 16;
const auto tree = rmi::KDTree<MyWrapperClassName>::for_mesh(mesh.begin(), mesh.end(), depth);
// const auto tree = rmi::Quadtree<MyWrapperClassName>::for_mesh(mesh.begin(), mesh.end(), depth);
// const auto tree = rmi::Octree<MyWrapperClassName>::for_mesh(mesh.begin(), mesh.end(), depth);
const rmi::Ray<my_float_t> ray(
    rmi::Vector3<my_float_t>(...), // origin
    rmi::Vector3<my_float_t>(...)  // direction
);
std::vector<rmi::Vector3<my_float_t>> points = ray.intersects(tree);
```

### Or use parallel algorithms [rmi_parallel.hpp](include/rmilib/rmi_parallel.hpp) (pool requires [external/wsq.hpp](external/wsq.hpp))
```cpp
#define RMI_INCLUDE_OMP
#define RMI_INCLUDE_POOL
#include "rmi_parallel.hpp"

const int threads_count = 4;
constexpr int splits_count = 1;

const auto tree = rmi::parallel::omp_build<MyWrapperClassName, splits_count>(mesh.begin(), mesh.end(), threads_count, depth);

std::vector<rmi::Vector3<my_float_t>> points = rmi::parallel::omp_intersects(ray, tree, threads_count);
std::vector<rmi::Vector3<my_float_t>> points = rmi::parallel::pool_intersects(ray, tree, threads_count);
```

## Build
```
cmake -S . -B build [-DBUILD_TESTS=on] [-DBUILD_QT_APP=on] [-DINCLUDE_OMP=ON] [-DINCLUDE_POOL=ON]
cd build
make -j%
```
### WASM (emscripten < 3.1.61)
```
emcmake cmake -S . -B build [-DBUILD_WASM=on]
cd build
make -j%
```

## Tests
```
ctest -j%
ctest -j% -L benchmark
ctest -j% -L unittest
```
