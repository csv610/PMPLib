// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/smoothing.h>

#include <iostream>
#include <getopt.h>

using namespace pmp;

void usage_and_exit()
{
    std::cerr << "Usage: meshfilter [options] -i <input> -o <output>\n\n"
              << "Apply Laplacian smoothing to a polygonal mesh.\n\n"
              << "Options:\n"
              << "  -h            show this help message\n"
              << "  -i <file>     input mesh file (required)\n"
              << "  -o <file>     output mesh file (required)\n"
              << "  -n <iter>     number of smoothing iterations (default: 10)\n"
              << "  -t <time>     timestep for implicit smoothing (default: 0.001)\n"
              << "  -u            use uniform Laplacian weights (default: cotan)\n"
              << "  -s            use implicit smoothing (default: explicit)\n"
              << "  -r            do not rescale after smoothing (default: rescale)\n"
              << "\n"
              << "Smoothing methods:\n"
              << "  explicit: iterative Laplacian smoothing (faster, less stable)\n"
              << "  implicit: implicit integration (more stable, -t controls timestep)\n"
              << "\n"
              << "Laplacian weights:\n"
              << "  uniform: uniform weights (equal to 1)\n"
              << "  cotan: cotangent weights (default, better for triangles)\n"
              << "\n"
              << "Example:\n"
              << "  meshfilter -i input.off -o output.off -n 20\n"
              << "  meshfilter -i input.off -o output.off -s -t 0.01 -n 5\n";
    exit(1);
}

int main(int argc, char** argv)
{
    const char* input = nullptr;
    const char* output = nullptr;
    unsigned int iterations = 10;
    Scalar timestep = 0.001;
    bool use_uniform = false;
    bool use_implicit = false;
    bool rescale = true;

    int c;
    while ((c = getopt(argc, argv, "hi:o:n:t:usr")) != -1)
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
            case 't':
                timestep = std::stod(optarg);
                break;
            case 'u':
                use_uniform = true;
                break;
            case 's':
                use_implicit = true;
                break;
            case 'r':
                rescale = false;
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

    std::cout << "Input: " << input << "\n";
    std::cout << "Vertices: " << mesh.n_vertices() << "\n";

    if (use_implicit)
    {
        std::cout << "Running implicit smoothing: " << iterations
                  << " iters, timestep=" << timestep
                  << ", uniform=" << use_uniform << "..." << std::endl;
        implicit_smoothing(mesh, timestep, iterations, use_uniform, rescale);
    }
    else
    {
        std::cout << "Running explicit smoothing: " << iterations
                  << " iters, uniform=" << use_uniform << "..." << std::endl;
        explicit_smoothing(mesh, iterations, use_uniform);
    }

    std::cout << "Output vertices: " << mesh.n_vertices() << std::endl;

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