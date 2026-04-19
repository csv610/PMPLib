// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/remeshing.h>

#include <iostream>
#include <getopt.h>

using namespace pmp;

void usage_and_exit()
{
    std::cerr << "Usage: meshremesh [options] -i <input> -o <output>\n\n"
              << "Remesh a polygonal mesh with improved triangle quality.\n\n"
              << "Options:\n"
              << "  -h              show this help message\n"
              << "  -i <file>       input mesh file (required)\n"
              << "  -o <file>      output mesh file (required)\n"
              << "  -m <mode>      remeshing mode:\n"
              << "                   uniform  - uniform edge length\n"
              << "                   adaptive - adaptive edge lengths (default)\n"
              << "  -l <len>       target edge length for uniform remeshing\n"
              << "  -n <len>       min edge length for adaptive remeshing\n"
              << "  -x <len>       max edge length for adaptive remeshing\n"
              << "  -e <error>     max approximation error for adaptive\n"
              << "  -r <iter>      number of iterations (default: 10)\n"
              << "  -p              disable projection to original surface\n"
              << "  -b              write output in binary format\n"
              << "\n"
              << "Uniform remeshing:\n"
              << "  Creates uniform mesh with specified edge length\n"
              << "  Use -l to set target edge length\n"
              << "\n"
              << "Adaptive remeshing:\n"
              << "  Uses variable edge length based on curvature/size\n"
              << "  -n: minimum edge length (preserve detail)\n"
              << "  -x: maximum edge length (smooth areas)\n"
              << "  -e: approximation error tolerance\n"
              << "\n"
              << "Other options:\n"
              << "  -r: more iterations = better quality (slower)\n"
              << "  -p: disable projection to keep original positions\n"
              << "\n"
              << "Example:\n"
              << "  meshremesh -i input.off -o output.off -m uniform -l 0.01\n"
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

    int c;
    while ((c = getopt(argc, argv, "hi:o:m:l:n:x:e:r:pb")) != -1)
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
        std::cerr << "Error: -l <edge_length> required for uniform remeshing\n";
        exit(1);
    }

    if (mode == "adaptive" && (min_edge_length <= 0.0 || max_edge_length <= 0.0))
    {
        std::cerr << "Error: -n <min> and -x <max> required for adaptive remeshing\n";
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

    std::cout << "Output vertices: " << mesh.n_vertices() << std::endl;
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