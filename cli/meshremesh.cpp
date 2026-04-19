// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/remeshing.h>

#include <iostream>
#include <getopt.h>

using namespace pmp;

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"input", required_argument, 0, 'i'},
    {"output", required_argument, 0, 'o'},
    {"mode", required_argument, 0, 'm'},
    {"length", required_argument, 0, 'l'},
    {"min-length", required_argument, 0, 'n'},
    {"max-length", required_argument, 0, 'x'},
    {"error", required_argument, 0, 'e'},
    {"iterations", required_argument, 0, 'r'},
    {"no-projection", no_argument, 0, 'p'},
    {"binary", no_argument, 0, 'b'},
    {0, 0, 0, 0}
};

void usage_and_exit()
{
    std::cerr << "Usage: meshremesh [options] --input <input> --output <output>\n\n"
              << "Remesh a polygonal mesh with improved triangle quality.\n\n"
              << "Options:\n"
              << "  -h, --help              show this help message\n"
              << "  -i, --input <file>      input mesh file (required)\n"
              << "  -o, --output <file>    output mesh file (required)\n"
              << "  -m, --mode <mode>      remeshing mode:\n"
              << "                           uniform  - uniform edge length\n"
              << "                           adaptive - adaptive edge lengths (default)\n"
              << "  -l, --length <len>     target edge length for uniform\n"
              << "  -n, --min-length <l>  min edge length for adaptive\n"
              << "  -x, --max-length <l>  max edge length for adaptive\n"
              << "  -e, --error <err>      approximation error for adaptive\n"
              << "  -r, --iterations <n>   number of iterations (default: 10)\n"
              << "  -p, --no-projection    disable projection to original surface\n"
              << "  -b, --binary         write binary format\n"
              << "\n"
              << "Uniform remeshing:\n"
              << "  --length: target edge length\n"
              << "\n"
              << "Adaptive remeshing:\n"
              << "  --min-length: preserve detail (small edges)\n"
              << "  --max-length: smooth areas (large edges)\n"
              << "  --error: approximation error limit\n"
              << "\n"
              << "Example:\n"
              << "  meshremesh --input input.off --output output.off --mode uniform --length 0.01\n"
              << "  meshremesh -i input.off -o output.off -m adaptive -n 0.005 -x 0.02 -e 0.001\n";
    exit(1);
}

int main(int argc, char** argv)
{
    const char* input = nullptr;
    const char* output = nullptr;
    std::string mode = "adaptive";
    Scalar edge_length = 0.0;
    Scalar min_edge_length = 0.0;
    Scalar max_edge_length = 0.0;
    Scalar approx_error = 0.0;
    unsigned int iterations = 10;
    bool use_projection = true;
    bool binary = false;

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "hi:o:m:l:n:x:e:r:pb", long_options, &option_index)) != -1)
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
                mode = optarg;
                break;
            case 'l':
                edge_length = std::stod(optarg);
                break;
            case 'n':
                min_edge_length = std::stod(optarg);
                break;
            case 'x':
                max_edge_length = std::stod(optarg);
                break;
            case 'e':
                approx_error = std::stod(optarg);
                break;
            case 'r':
                iterations = std::stoul(optarg);
                break;
            case 'p':
                use_projection = false;
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

    if (mode == "uniform" && edge_length <= 0.0)
    {
        std::cerr << "Error: --length required for uniform remeshing\n";
        exit(1);
    }

    if (mode == "adaptive" && (min_edge_length <= 0.0 || max_edge_length <= 0.0))
    {
        std::cerr << "Error: --min-length and --max-length required for adaptive\n";
        exit(1);
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
    std::cout << "Mode: " << mode << ", iterations: " << iterations << "\n";

    try
    {
        if (mode == "uniform")
        {
            std::cout << "Edge length: " << edge_length << "\n";
            uniform_remeshing(mesh, edge_length, iterations, use_projection);
        }
        else if (mode == "adaptive")
        {
            std::cout << "Min edge: " << min_edge_length
                    << ", Max edge: " << max_edge_length
                    << ", Approx error: " << approx_error << "\n";
            adaptive_remeshing(mesh, min_edge_length, max_edge_length,
                            approx_error, iterations, use_projection);
        }
        else
        {
            std::cerr << "Unknown mode: " << mode << "\n";
            exit(1);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Remeshing failed: " << e.what() << std::endl;
        exit(1);
    }

    std::cout << "Output vertices: " << mesh.n_vertices() << "\n";
    std::cout << "Faces: " << mesh.n_faces() << std::endl;

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