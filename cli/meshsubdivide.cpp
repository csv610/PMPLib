// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/subdivision.h>

#include <iostream>
#include <getopt.h>

using namespace pmp;

void usage_and_exit()
{
    std::cerr << "Usage: meshsubdivide [options] -i <input> -o <output>\n\n"
              << "Subdivide a polygonal mesh to create a smoother surface.\n\n"
              << "Options:\n"
              << "  -h            show this help message\n"
              << "  -i <file>     input mesh file (required)\n"
              << "  -o <file>    output mesh file (required)\n"
              << "  -n <iter>    number of subdivision iterations (default: 1)\n"
              << "  -m <method>  subdivision method:\n"
              << "                 cc     - Catmull-Clark (default, for quad/ngon meshes)\n"
              << "                 loop   - Loop (for triangle meshes only)\n"
              << "                 qt     - quad-tri subdivision\n"
              << "                 linear - linear quad-tri subdivision\n"
              << "  -b <mode>   boundary handling:\n"
              << "                 i - interpolate boundary (default)\n"
              << "                 p - preserve boundary positions\n"
              << "\n"
              << "Subdivision methods:\n"
              << "  cc:    Catmull-Clark - best for quad meshes, also works with triangles\n"
              << "  loop:  Loop - best for triangle meshes, creates smooth C2 surface\n"
              << "  qt:    Quad-tri - mixed quad/triangle meshes\n"
              << "  linear: Linear - no smoothing, just splits edges\n"
              << "\n"
              << "Example:\n"
              << "  meshsubdivide -i input.off -o output.off -n 2\n"
              << "  meshsubdivide -i input.off -o output.off -m loop -n 1\n";
    exit(1);
}

int main(int argc, char** argv)
{
    const char* input = nullptr;
    const char* output = nullptr;
    unsigned int iterations = 1;
    std::string method = "cc";
    char boundary = 'i';

    int c;
    while ((c = getopt(argc, argv, "hi:o:n:m:b:")) != -1)
    {
        switch (c)
        {
            case 'h':
                usage_and_exit();
                break;
            case 'i':
                input = optarg;
                break;
            case 'o':
                output = optarg;
                break;
            case 'n':
                iterations = std::stoul(optarg);
                break;
            case 'm':
                method = optarg;
                break;
            case 'b':
                boundary = optarg[0];
                break;
            default:
                usage_and_exit();
        }
    }

    if (!input || !output)
    {
        usage_and_exit();
    }

    SurfaceMesh mesh;
    try
    {
        read(mesh, input);
    }
    catch (const IOException& e)
    {
        std::cerr << "Failed to read mesh: " << e.what() << std::endl;
        exit(1);
    }

    BoundaryHandling bh = (boundary == 'p')
                            ? BoundaryHandling::Preserve
                            : BoundaryHandling::Interpolate;

    unsigned int orig_verts = mesh.n_vertices();
    std::cout << "Input: " << input << "\n";
    std::cout << "Vertices: " << orig_verts << "\n";
    std::cout << "Method: " << method << ", iterations: " << iterations << "\n";

    for (unsigned int i = 0; i < iterations; ++i)
    {
        if (method == "loop")
        {
            loop_subdivision(mesh, bh);
        }
        else if (method == "qt")
        {
            quad_tri_subdivision(mesh, bh);
        }
        else if (method == "linear")
        {
            linear_subdivision(mesh);
        }
        else
        {
            catmull_clark_subdivision(mesh, bh);
        }
        std::cout << "Step " << (i + 1) << ": " << mesh.n_vertices()
                 << " vertices\n";
    }

    try
    {
        write(mesh, output);
    }
    catch (const IOException& e)
    {
        std::cerr << "Failed to write mesh: " << e.what() << std::endl;
        exit(1);
    }

    std::cout << "Saved to: " << output << std::endl;
    return 0;
}