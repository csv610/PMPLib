// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/decimation.h>

#include <iostream>
#include <getopt.h>

using namespace pmp;

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"input", required_argument, 0, 'i'},
    {"output", required_argument, 0, 'o'},
    {"vertices", required_argument, 0, 'n'},
    {"aspect-ratio", required_argument, 0, 'a'},
    {"edge-length", required_argument, 0, 'e'},
    {"max-valence", required_argument, 0, 'm'},
    {"normal-deviation", required_argument, 0, 'd'},
    {"hausdorff", required_argument, 0, 'H'},
    {0, 0, 0, 0}
};

void usage_and_exit()
{
    std::cerr << "Usage: meshsimplify [options] --input <input> --output <output>\n\n"
              << "Simplify a mesh using quadric-based decimation.\n\n"
              << "Options:\n"
              << "  -h, --help              show this help message\n"
              << "  -i, --input <file>      input mesh file (required)\n"
              << "  -o, --output <file>     output mesh file (required)\n"
              << "  -n, --vertices <num>    target number of vertices (required)\n"
              << "  -a, --aspect-ratio <r>  min aspect ratio [0-1] (default: 0)\n"
              << "  -e, --edge-length <l>   min edge length (default: 0)\n"
              << "  -m, --max-valence <v>  max vertex valence (default: 0)\n"
              << "  -d, --normal-deviation <deg>  max normal deviation in degrees (default: 0)\n"
              << "  -H, --hausdorff <err>    max Hausdorff error (default: 0)\n"
              << "\n"
              << "Constraints:\n"
              << "  --aspect-ratio: minimum triangle aspect ratio (higher = more regular)\n"
              << "  --edge-length: minimum edge length to preserve details\n"
              << "  --max-valence: maximum vertex valence (default: unlimited)\n"
              << "  --normal-deviation: preserves sharp features\n"
              << "  --hausdorff: limits deviation from original surface\n"
              << "\n"
              << "Example:\n"
              << "  meshsimplify --input input.off --output output.off --vertices 1000\n"
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

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "hi:o:n:a:e:m:d:H:", long_options, &option_index)) != -1)
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