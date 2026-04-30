// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/subdivision.h>

#include <argparse/argparse.hpp>

#include <iostream>
#include <sstream>

using namespace pmp;

unsigned int parse_uint(const std::string& s)
{
    unsigned int value = 0;
    std::istringstream iss(s);
    iss >> value;
    return value;
}

int main(int argc, char** argv)
{
    argparse::ArgumentParser program("meshsubdivide", "1.0");

    std::string input_file;
    std::string output_file;
    unsigned int iterations = 1;
    std::string method = "cc";
    char boundary = 'i';

    program.add_argument("-i", "--input")
        .help("Input mesh file")
        .required();

    program.add_argument("-o", "--output")
        .help("Output mesh file")
        .required();

    program.add_argument("-n", "--iterations")
        .help("Number of iterations")
        .default_value("1");

    program.add_argument("-m", "--method")
        .help("Subdivision method: cc, loop, qt, linear")
        .default_value("cc");

    program.add_argument("-b", "--boundary")
        .help("Boundary handling: i (interpolate), p (preserve)")
        .default_value("i");

    program.add_argument("-h", "--help")
        .help("shows help message and exits")
        .default_value(false)
        .implicit_value(true);

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

    if (program.get<bool>("--help"))
    {
        std::cout << program;
        return 0;
    }

    try
    {
        input_file = program.get<std::string>("--input");
        output_file = program.get<std::string>("--output");
        iterations = parse_uint(program.get<std::string>("--iterations"));
        method = program.get<std::string>("--method");
        std::string boundary_str = program.get<std::string>("--boundary");
        if (!boundary_str.empty())
            boundary = boundary_str[0];
    }
    catch (const std::exception& err)
    {
        std::cerr << "Error getting arguments: " << err.what() << std::endl;
        return 1;
    }

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

    BoundaryHandling bh = (boundary == 'p') ? BoundaryHandling::Preserve : BoundaryHandling::Interpolate;

    unsigned int orig_verts = mesh.n_vertices();
    std::cout << "Input: " << input_file << "\n";
    std::cout << "Vertices: " << orig_verts << "\n";
    std::cout << "Method: " << method << ", iterations: " << iterations << "\n";

    for (unsigned int i = 0; i < iterations; ++i)
    {
        if (method == "loop")
            loop_subdivision(mesh, bh);
        else if (method == "qt")
            quad_tri_subdivision(mesh, bh);
        else if (method == "linear")
            linear_subdivision(mesh);
        else
            catmull_clark_subdivision(mesh, bh);
        std::cout << "Step " << (i + 1) << ": " << mesh.n_vertices() << " vertices\n";
    }

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