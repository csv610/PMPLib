// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/parameterization.h>

#include <iostream>
#include <getopt.h>

using namespace pmp;

void usage_and_exit()
{
    std::cerr << "Usage: meshparam [options] -i <input> -o <output>\n\n"
              << "Compute 2D parameterization for UV mapping of a polygonal mesh.\n\n"
              << "Options:\n"
              << "  -h            show this help message\n"
              << "  -i <file>     input mesh file (required)\n"
              << "  -o <file>    output mesh file (required)\n"
              << "  -m <method>  parameterization method:\n"
              << "                 harmonic - harmonic parameterization (default)\n"
              << "                 lscm     - least squares conformal mapping\n"
              << "  -u            use uniform weights for harmonic (default: cotangent)\n"
              << "  -b            write output in binary format (default: ASCII)\n"
              << "\n"
              << "Methods:\n"
              << "  harmonic: Discrete harmonic parameterization (DHP)\n"
              << "           - Fast, requires bounded mesh\n"
              << "           - Preserves angles poorly\n"
              << "  lscm:     Least Squares Conformal Mapping\n"
              << "           - Conformal (angle-preserving)\n"
              << "           - Two-free, requires triangle mesh\n"
              << "\n"
              << "Requirements:\n"
              << "  - Mesh must have a boundary (outer edge loop)\n"
              << "  - lscm requires triangle mesh\n"
              << "\n"
              << "Output:\n"
              << "  UV coordinates are stored as texture coordinates in output mesh\n"
              << "  Can be visualized or exported to texture files\n"
              << "\n"
              << "Example:\n"
              << "  meshparam -i input.off -o output.off -m harmonic\n"
              << "  meshparam -i input.off -o output.off -m lscm\n";
    exit(1);
}

int main(int argc, char** argv)
{
    const char* input = nullptr;
    const char* output = nullptr;
    std::string method = "harmonic";
    bool use_uniform = false;
    bool binary = false;

    int c;
    while ((c = getopt(argc, argv, "hi:o:m:ub")) != -1)
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
            case 'u':
                use_uniform = true;
                break;
            case 'b':
                binary = true;
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
        if (method == "lscm")
        {
            std::cout << "Computing LSCM parameterization...\n";
            lscm_parameterization(mesh);
        }
        else if (method == "harmonic")
        {
            std::cout << "Computing harmonic parameterization...\n";
            harmonic_parameterization(mesh, use_uniform);
        }
        else
        {
            std::cerr << "Unknown method: " << method << "\n";
            exit(1);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Parameterization failed: " << e.what() << std::endl;
        exit(1);
    }

    std::cout << "Output vertices: " << mesh.n_vertices() << std::endl;

    try
    {
        IOFlags flags;
        flags.use_binary = binary;
        write(mesh, output, flags);
    }
    catch (const IOException& e)
    {
        std::cerr << "Failed to write mesh: " << e.what() << std::endl;
        exit(1);
    }

    std::cout << "Saved to: " << output << std::endl;
    return 0;
}