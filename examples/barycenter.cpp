// Copyright 2013-2017 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>

using namespace pmp;

// clang-format off
int main(int argc, char** argv)
{
    if (argc > 1 && std::string(argv[1]) == "-h")
    {
        std::cerr << "Usage: " << argv[0] << " [input]\n";
        std::cerr << "  Compute and print the barycenter of a mesh.\n";
        exit(0);
    }

    SurfaceMesh mesh;

if (argc > 1)
    read(mesh, argv[1]);

//! [barycenter]
// get pre-defined property storing vertex positions
auto points = mesh.get_vertex_property<Point>("v:point");

Point p(0, 0, 0);

for (auto v : mesh.vertices())
{
    // access point property like an array
    p += points[v];
}

p /= mesh.n_vertices();

std::cout << "barycenter: " << p << std::endl;
//! [barycenter]
}
// clang-format on
