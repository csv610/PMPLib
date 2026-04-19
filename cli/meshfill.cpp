// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/hole_filling.h>

#include <iostream>
#include <getopt.h>
#include <vector>

using namespace pmp;

void usage_and_exit()
{
    std::cerr << "Usage: meshfill [options] -i <input> -o <output>\n\n"
              << "Fill holes in a polygonal mesh using triangulation and fairing.\n\n"
              << "Options:\n"
              << "  -h            show this help message\n"
              << "  -i <file>     input mesh file (required)\n"
              << "  -o <file>    output mesh file (required)\n"
              << "  -a            fill all holes (default: fill specified hole)\n"
              << "  -n <num>     hole index to fill, 0-based (default: 0)\n"
              << "\n"
              << "Algorithm:\n"
              << "  Fills holes by first triangulating the boundary loop,\n"
              << "  then performs isometric remeshing, and finally applies\n"
              << "  curvature-minimizing fairing to the filled patch.\n"
              << "\n"
              << "Notes:\n"
              << "  - Only fills manifold boundary loops\n"
              << "  - Non-manifold boundaries are skipped\n"
              << "  - Use meshinfo first to see available holes\n"
              << "\n"
              << "Example:\n"
              << "  meshfill -i input.off -o output.off -a\n"
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

    int c;
    while ((c = getopt(argc, argv, "hi:o:an:")) != -1)
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