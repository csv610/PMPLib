// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/parameterization.h>

#include <iostream>
#include <getopt.h>

using namespace pmp;

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"input", required_argument, 0, 'i'},
    {"output", required_argument, 0, 'o'},
    {"method", required_argument, 0, 'm'},
    {"uniform", no_argument, 0, 'u'},
    {"binary", no_argument, 0, 'b'},
    {0, 0, 0, 0}
};

void usage_and_exit()
{
    std::cerr << "Usage: meshparam [options] --input <input> --output <output>\n\n"
              << "Compute 2D parameterization for UV mapping.\n\n"
              << "Options:\n"
              << "  -h, --help            show this help message\n"
              << "  -i, --input <file>     input mesh file (required)\n"
              << "  -o, --output <file>   output mesh file (required)\n"
              << "  -m, --method <m>     parameterization method:\n"
              << "                       harmonic - harmonic (default)\n"
              << "                       lscm     - least squares conformal\n"
              << "  -u, --uniform        use uniform weights (default: cotangent)\n"
              << "  -b, --binary        write binary format\n"
              << "\n"
              << "Methods:\n"
              << "  harmonic: Fast, requires bounded mesh, angles not preserved\n"
              << "  lscm:     Conformal, requires triangle mesh\n"
              << "\n"
              << "Requirements:\n"
              << "  - Mesh must have a boundary\n"
              << "  - lscm requires triangle mesh\n"
              << "\n"
              << "Example:\n"
              << "  meshparam --input input.off --output output.off --method harmonic\n"
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

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "hi:o:m:ub", long_options, &option_index)) != -1)
    {
        switch (opt)
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