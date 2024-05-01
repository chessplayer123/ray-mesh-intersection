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
