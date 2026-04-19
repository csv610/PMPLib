// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/decimation.h>

#include <iostream>
#include <getopt.h>

using namespace pmp;

void usage_and_exit()
{
    std::cerr << "Usage: meshsimplify [options] -i <input> -o <output>\n\n"
              << "Simplify a mesh using quadric-based decimation.\n\n"
              << "Options:\n"
              << "  -h            show this help message\n"
              << "  -i <file>     input mesh file (required)\n"
              << "  -o <file>    output mesh file (required)\n"
              << "  -n <num>     target number of vertices (required)\n"
              << "  -a <ratio>  min aspect ratio of triangles [0-1] (default: 0)\n"
              << "  -e <len>    min edge length (default: 0)\n"
              << "  -m <val>    max vertex valence (default: 0)\n"
              << "  -d <deg>    max normal deviation in degrees (default: 0)\n"
              << "  -H <err>    max Hausdorff approximation error (default: 0)\n"
              << "\n"
              << "Constraints (use to control quality):\n"
              << "  -a: minimum triangle aspect ratio (higher = more regular triangles)\n"
              << "  -e: minimum edge length to preserve details\n"
              << "  -m: maximum vertex valence (default: unlimited)\n"
              << "  -d: normal deviation preserves sharp features\n"
              << "  -H: limits deviation from original surface\n"
              << "\n"
              << "Example:\n"
              << "  meshsimplify -i input.off -o output.off -n 1000\n"
              << "  meshsimplify -i input.off -o output.off -n 500 -a 0.5 -d 30\n";
    exit(1);
}

int main(int argc, char** argv)
{
    const char* input = nullptr;
    const char* output = nullptr;
    unsigned int n_vertices = 0;
    Scalar aspect_ratio = 0.0;
    Scalar edge_length = 0.0;
    unsigned int max_valence = 0;
    Scalar normal_deviation = 0.0;
    Scalar hausdorff_error = 0.0;

    int c;
    while ((c = getopt(argc, argv, "hi:o:n:a:e:m:d:H:")) != -1)
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
                n_vertices = std::stoul(optarg);
                break;
            case 'a':
                aspect_ratio = std::stod(optarg);
                break;
            case 'e':
                edge_length = std::stod(optarg);
                break;
            case 'm':
                max_valence = std::stoul(optarg);
                break;
            case 'd':
                normal_deviation = std::stod(optarg);
                break;
            case 'H':
                hausdorff_error = std::stod(optarg);
                break;
            default:
                usage_and_exit();
        }
    }

    if (!input || !output || n_vertices == 0)
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

    unsigned int orig_verts = mesh.n_vertices();
    std::cout << "Input: " << input << "\n";
    std::cout << "Vertices: " << orig_verts << "\n";
    std::cout << "Target: " << n_vertices << "\n";

    try
    {
        decimate(mesh, n_vertices, aspect_ratio, edge_length, max_valence,
                normal_deviation, hausdorff_error);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Decimation failed: " << e.what() << std::endl;
        exit(1);
    }

    unsigned int new_verts = mesh.n_vertices();
    std::cout << "Output vertices: " << new_verts
             << " (" << (100.0 * new_verts / orig_verts) << "%)\n";

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