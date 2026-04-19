// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/fairing.h>

#include <iostream>
#include <getopt.h>

using namespace pmp;

void usage_and_exit()
{
    std::cerr << "Usage: meshfair [options] -i <input> -o <output>\n\n"
              << "Fair (smooth) a mesh by minimizing curvature or surface area.\n\n"
              << "Options:\n"
              << "  -h            show this help message\n"
              << "  -i <file>     input mesh file (required)\n"
              << "  -o <file>    output mesh file (required)\n"
              << "  -m <method>  fairing method:\n"
              << "                 area   - minimize surface area (default)\n"
              << "                 curv   - minimize surface curvature\n"
              << "                 fair   - implicit fairing (k-harmonic)\n"
              << "  -k <order>   order for implicit fairing (2-4, default: 2)\n"
              << "\n"
              << "Fairing methods:\n"
              << "  area:  Minimizes total surface area (first-order gradient flow)\n"
              << "  curv:  Minimizes curvature (second-order derivative)\n"
              << "  fair:  Solves k-harmonic equation (most smoothing)\n"
              << "\n"
              << "Implicit fairing (-m fair):\n"
              << "  -k 2: solves biharmonic equation (default)\n"
              << "  -k 3: solves triharmonic equation\n"
              << "  -k 4: solves quadharmonic equation\n"
              << "\n"
              << "Example:\n"
              << "  meshfair -i input.off -o output.off -m curv\n"
              << "  meshfair -i input.off -o output.off -m fair -k 3\n";
    exit(1);
}

int main(int argc, char** argv)
{
    const char* input = nullptr;
    const char* output = nullptr;
    std::string method = "curv";
    unsigned int order = 2;

    int c;
    while ((c = getopt(argc, argv, "hi:o:m:k:")) != -1)
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
            case 'm':
                method = optarg;
                break;
            case 'k':
                order = std::stoul(optarg);
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
    std::cout << "Method: " << method << "\n";

    try
    {
if (method == "area")
        {
            std::cout << "Minimizing surface area...\n";
            minimize_area(mesh);
        }
        else if (method == "curv")
        {
            std::cout << "Minimizing curvature...\n";
            minimize_curvature(mesh);
        }
        else if (method == "fair")
        {
            std::cout << "Implicit fairing (k=" << order << ")...\n";
            fair(mesh, order);
        }
        else
        {
            std::cerr << "Unknown method: " << method << "\n";
            exit(1);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fairing failed: " << e.what() << std::endl;
        exit(1);
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