# PMPLib - Polygon Mesh Processing Library

[![Build](https://github.com/csv610/PMPLib/actions/workflows/build/badge.svg)](https://github.com/csv610/PMPLib/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

PMPLib is a modern C++ library for processing polygon surface meshes. This is a fork/port of the original [pmp-library](https://github.com/pmp-library/pmp-library) with additional CLI tools for command-line mesh processing.

## Features

- **Efficient mesh data structure** - Half-edge data structure for polygon meshes
- **Comprehensive algorithms** - Decimation, remeshing, subdivision, smoothing, hole filling, fairing, parameterization
- **CLI tools** - 8 command-line applications for batch processing
- **Cross-platform** - Windows, macOS, Linux
- **Header-only optional** - Can be used as header-only library

## CLI Tools (New in this port)

This port adds 8 command-line tools for mesh processing:

| Tool | Description |
|------|-------------|
| `meshinfo` | Display mesh statistics (vertices, faces, bbox, area, curvature, features) |
| `meshfilter` | Laplacian smoothing (explicit/implicit, uniform/cotan weights) |
| `meshsimplify` | Quadric-based mesh decimation |
| `meshsubdivide` | Catmull-Clark, Loop, quad-tri subdivision |
| `meshfill` | Hole filling with triangulation and fairing |
| `meshfair` | Surface fairing (area/curvature minimization, k-harmonic) |
| `meshparam` | UV parameterization (harmonic, LSCM) |
| `meshremesh` | Uniform/adaptive remeshing |

### Building CLI Tools

```sh
mkdir build && cd build
cmake .. -DPMP_BUILD_CLI=ON
make
```

### Using CLI Tools

```sh
# Get mesh information
./meshinfo input.off

# Smooth a mesh
./meshfilter -i input.off -o output.off -n 20

# Simplify mesh to 1000 vertices
./meshsimplify -i input.off -o output.off -n 1000

# Subdivide mesh
./meshsubdivide -i input.off -o output.off -n 2

# Fill holes
./meshfill -i input.off -o output.off -a

# Fair mesh
./meshfair -i input.off -o output.off -m curv

# UV mapping
./meshparam -i input.off -o output.off -m harmonic

# Remesh
./meshremesh -i input.off -o output.off -m uniform -l 0.01
```

## Comparison with Other Libraries

| Feature | PMPLib | libigl | CGAL |
|---------|-------|--------|------|
| **Language** | C++ | C++ | C++ |
| **Dependencies** | Eigen only | Eigen | Boost, GMP |
| **License** | MIT | MPL2 | GPL3/LGPL |
| **Mesh Data Structure** | Half-edge | Custom | CGAL::Surface_mesh |
| **CLI Tools** | 8 included | None | Some (via package) |
| **Header-only** | Optional | Yes | No |
| **Compilation** | Fast | Fast | Slow |
| **Template Mesh** | No | Yes | Yes |
| **GUI Viewer** | GLFW-based | Matplotlib/glfw | Viewer3 |

### When to Use PMPLib

- **Choose PMPLib** when you need: fast compilation, minimal dependencies, CLI tools for batch processing, simple API
- **Choose libigl** when you need: header-only convenience, template meshes, MATLAB-like syntax, tutorial-style documentation
- **Choose CGAL** when you need: robust geometry kernels, exact arithmetic, advanced computational geometry

### Algorithm Coverage

| Algorithm | PMPLib | libigl | CGAL |
|-----------|-------|--------|------|
| Subdivision | Yes | Yes | Yes |
| Smoothing | Yes | Yes | Yes |
| Decimation | Yes | Yes | Yes |
| Remeshing | Yes | Yes | Partial |
| Hole Filling | Yes | No | Yes |
| Parameterization | Yes | Yes | Yes |
| Fairing | Yes | Yes | Yes |
| Curvature | Yes | Yes | Yes |

## Get Started

### Clone and Build

```sh
git clone https://github.com/csv610/PMPLib.git
cd PMPLib
mkdir build && cd build
cmake .. -DPMP_BUILD_CLI=ON
make
```

### Use as Library

```cpp
#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>

int main()
{
    pmp::SurfaceMesh mesh;
    pmp::read(mesh, "input.obj");
    // process mesh
    pmp::write(mesh, "output.obj");
}
```

## Documentation

- [PMP Library Guide](https://www.pmp-library.org/guide.html) - Original library documentation
- [CLI Help](#cli-tools) - Use `-h` flag with each tool

## License

PMPLib is provided under the [MIT License](LICENSE.txt), same as the original pmp-library.

## Acknowledgments

This library is based on the [pmp-library](https://github.com/pmp-library/pmp-library) by Daniel Sieger and Mario Botsch. PMPLib adds CLI tools for command-line usage while maintaining compatibility with the original library.

## Contributing

CLI tools in this port were added by [csv610](https://github.com/csv610). Contributions are welcome - please submit pull requests or open issues for bugs and feature requests.

## Citation

If you use this library, please cite the original pmp-library:

```tex
@software{pmp23,
  author = {Sieger, Daniel and Botsch, Mario},
  title = {{The Polygon Mesh Processing Library}},
  year = {2023},
  version = {3.0.0},
  url = {https://github.com/pmp-library/pmp-library}
}
```