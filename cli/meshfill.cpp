// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/hole_filling.h>

#include <argparse/argparse.hpp>

#include <iostream>
#include <vector>

using namespace pmp;

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
                loops.push_back(loop);
        }
    }
    return loops;
}

int main(int argc, char** argv)
{
    argparse::ArgumentParser program("meshfill", "1.0", argparse::default_arguments::help);

    std::string input_file;
    std::string output_file;
    bool fill_all = false;
    int hole_num = 0;

    program.add_argument("-i", "--input")
        .help("Input mesh file")
        .required();

    program.add_argument("-o", "--output")
        .help("Output mesh file")
        .required();

    program.add_argument("-a", "--all")
        .help("Fill all holes")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-n", "--hole")
        .help("Hole index to fill (0-based)")
        .default_value(0)
        .scan<'i', int>();

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    input_file = program.get<std::string>("--input");
    output_file = program.get<std::string>("--output");
    fill_all = program.get<bool>("--all");
    hole_num = program.get<int>("--hole");

    SurfaceMesh mesh;
    try
    {
        read(mesh, input_file);
    }
    catch (const IOException& e)
    {
        std::cerr << "Failed to read mesh: " << e.what() << std::endl;
        return 1;
    }

    auto loops = find_boundary_loops(mesh);
    if (loops.empty())
    {
        std::cout << "No manifold boundary loops found.\n";
        write(mesh, output_file);
        std::cout << "Saved to: " << output_file << std::endl;
        return 0;
    }

    std::cout << "Found " << loops.size() << " boundary loop(s)\n";

    unsigned int target_hole = fill_all ? loops.size() : hole_num + 1;
    unsigned int filled = 0;
    for (unsigned int i = 0; i < loops.size() && i < target_hole; ++i)
    {
        try
        {
            fill_hole(mesh, loops[i].start);
            filled++;
            std::cout << "Filled hole " << i << " (" << loops[i].length << " edges)\n";
        }
        catch (const std::exception& e)
        {
            std::cerr << "Failed to fill hole " << i << ": " << e.what() << "\n";
        }
    }

    std::cout << "Filled " << filled << " hole(s)\n";

    try
    {
        write(mesh, output_file);
    }
    catch (const IOException& e)
    {
        std::cerr << "Failed to write mesh: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Saved to: " << output_file << std::endl;
    return 0;
}