![build status](https://github.com/chessplayer123/ray-mesh-intersection/actions/workflows/cmake-multi-platform.yml/badge.svg)
# Ray And Triangular Mesh Intersection

## Project structure
- [include](include/): public header files
- [src](src/): library source files
- [app](app/): main application
- [test](test/): unittest and benchmark files
- [external](external/): external dependencies
- [data](data/): models


## Build
```
cmake -S . -B build [-DBUILD_TESTS=on] [-DBUILD_QT_APP=on] [-DBUILD_WASM=on] [-DBACKEND=NONE|POOL|OPENMP]
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
