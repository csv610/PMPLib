// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/hole_filling.h>

#include <iostream>
#include <getopt.h>
#include <vector>

using namespace pmp;

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"input", required_argument, 0, 'i'},
    {"output", required_argument, 0, 'o'},
    {"all", no_argument, 0, 'a'},
    {"hole", required_argument, 0, 'n'},
    {0, 0, 0, 0}
};

void usage_and_exit()
{
    std::cerr << "Usage: meshfill [options] --input <input> --output <output>\n\n"
              << "Fill holes in a polygonal mesh using triangulation and fairing.\n\n"
              << "Options:\n"
              << "  -h, --help          show this help message\n"
              << "  -i, --input <file>  input mesh file (required)\n"
              << "  -o, --output <file> output mesh file (required)\n"
              << "  -a, --all          fill all holes (default: fill specified hole)\n"
              << "  -n, --hole <num>    hole index to fill, 0-based (default: 0)\n"
              << "\n"
              << "Algorithm:\n"
              << "  Fills holes by triangulating the boundary loop,\n"
              << "  performs isometric remeshing, and applies\n"
              << "  curvature-minimizing fairing.\n"
              << "\n"
              << "Notes:\n"
              << "  - Only fills manifold boundary loops\n"
              << "  - Non-manifold boundaries are skipped\n"
              << "  - Use meshinfo first to check holes\n"
              << "\n"
              << "Example:\n"
              << "  meshfill --input input.off --output output.off --all\n"
              << "  meshfill -i input.off -o output.off -n 0\n";
    exit(1);
}

struct BoundaryLoop
{
    Halfedge start;
    unsigned int length;
};

std::vector<BoundaryLoop> find_boundary_loops(const SurfaceMesh& mesh)
{
    std::vector<BoundaryLoop> loops;
    std::vector<bool> visited(mesh.n_halfedges(), false);

    for (auto h : mesh.halfedges())
    {
        if (mesh.is_boundary(h) && !visited[h.idx()])
        {
            BoundaryLoop loop;
            loop.start = h;
            loop.length = 0;

            Halfedge hh = h;
            do
            {
                visited[hh.idx()] = true;
                loop.length++;
                hh = mesh.next_halfedge(hh);
            } while (hh != h);

            if (mesh.is_manifold(mesh.to_vertex(h)))
            {
                loops.push_back(loop);
            }
        }
    }

    return loops;
}

int main(int argc, char** argv)
{
    const char* input = nullptr;
    const char* output = nullptr;
    bool fill_all = false;
    int hole_num = -1;

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "hi:o:an:", long_options, &option_index)) != -1)
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
            case 'a':
                fill_all = true;
                break;
            case 'n':
                hole_num = std::stoi(optarg);
                break;
            default:
                usage_and_exit();
        }
    }

    if (!input || !output)
    {
        usage_and_exit();
    }

    if (!fill_all && hole_num < 0)
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

    auto loops = find_boundary_loops(mesh);

    if (loops.empty())
    {
        std::cout << "No manifold boundary loops found.\n";
        write(mesh, output);
        std::cout << "Saved to: " << output << std::endl;
        return 0;
    }

    std::cout << "Found " << loops.size() << " boundary loop(s)\n";

    unsigned int target_hole = 0;
    if (fill_all)
    {
        target_hole = loops.size();
    }
    else
    {
        target_hole = hole_num + 1;
    }

    unsigned int filled = 0;
    for (unsigned int i = 0; i < loops.size() && i < target_hole; ++i)
    {
        try
        {
            fill_hole(mesh, loops[i].start);
            filled++;
            std::cout << "Filled hole " << i << " (" << loops[i].length
                    << " edges)\n";
        }
        catch (const std::exception& e)
        {
            std::cerr << "Failed to fill hole " << i << ": " << e.what() << "\n";
        }
    }

    std::cout << "Filled " << filled << " hole(s)\n";

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