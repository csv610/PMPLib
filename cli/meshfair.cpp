// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/fairing.h>

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
    argparse::ArgumentParser program("meshfair", "1.0");

    std::string input_file;
    std::string output_file;
    std::string method = "curv";
    unsigned int order = 2;

    program.add_argument("-i", "--input")
        .help("Input mesh file")
        .required();

    program.add_argument("-o", "--output")
        .help("Output mesh file")
        .required();

    program.add_argument("-m", "--method")
        .help("Fairing method: area, curv, fair")
        .default_value("curv");

    program.add_argument("-k", "--order")
        .help("Order for implicit fairing (2-4)")
        .default_value("2");

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
        method = program.get<std::string>("--method");
        order = parse_uint(program.get<std::string>("--order"));
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

    std::cout << "Input: " << input_file << "\n";
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
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fairing failed: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Output vertices: " << mesh.n_vertices() << std::endl;

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