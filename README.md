![build status](https://github.com/chessplayer123/ray-mesh-intersection/actions/workflows/cmake-multi-platform.yml/badge.svg)
# Ray And Triangular Mesh Intersection

## Project structure
- [include](include/): header files
- [src](src/): source files
- [apps](apps/): main application
- [test](test/): unit test files
- [external](external/): dependencies
- [data](data/): models


## Build
### Linux
```
cmake -S . -B build
cd build
make -j%
```

## Tests
```
ctest
```
```
make test
```
