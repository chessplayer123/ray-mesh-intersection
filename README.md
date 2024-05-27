![build status](https://github.com/chessplayer123/ray-mesh-intersection/actions/workflows/cmake-multi-platform.yml/badge.svg)
# Ray And Triangular Mesh Intersection Library

## Usage

### Create wrapper for mesh
```cpp
class MyWrapperClassName: public Mesh<MyWrapperClassName, my_float_t, my_index_t> {
public:
    MyWrapperClassName(...):
        Mesh<MyWrapperClassName, my_float_t, my_index_t>(size_of_mesh)
        ...
    {
       ...
    }

    template<my_index_t>
    inline Vector3<my_float_t> v(my_index_t index) const {
        ...
        return Vector3<my_float_t>(x, y, z);
    }
    ...
};
```

### Build KDTree and find intersections
```cpp
const MyWrapperClassName mesh(...);
const int depth = 16;
const auto kdtree = KDTree<MyWrapperClassName>::for_mesh(mesh.begin(), mesh.end(), depth);
const Ray<my_float_t> ray(
    Vector3<my_float_t>(...), // origin
    Vector3<my_float_t>(...)  // direction
);
std::vector<Vector3<my_float_t>> points = ray.intersects(kdtree);
```

## Project structure
- [include](include/): public header files
- [src](src/): library source files
- [app](app/): main application
- [test](test/): unittest and benchmark files
- [external](external/): external dependencies
- [data](data/): models

## Build
```
cmake -S . -B build [-DBUILD_TESTS=on] [-DBUILD_QT_APP=on] [-DINCLUDE_OMP=ON] [-DINCLUDE_POOL=ON]
cd build
make -j%
```
### WASM
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
