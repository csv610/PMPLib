// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/parameterization.h>

#include <argparse/argparse.hpp>

#include <iostream>

using namespace pmp;

int main(int argc, char** argv)
{
    argparse::ArgumentParser program("meshparam", "1.0");

    std::string input_file;
    std::string output_file;
    std::string method = "harmonic";
    bool use_uniform = false;
    bool binary = false;

    program.add_argument("-i", "--input")
        .help("Input mesh file")
        .required();

    program.add_argument("-o", "--output")
        .help("Output mesh file")
        .required();

    program.add_argument("-m", "--method")
        .help("Method: harmonic, lscm")
        .default_value("harmonic");

    program.add_argument("-u", "--uniform")
        .help("Use uniform weights")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-b", "--binary")
        .help("Write binary format")
        .default_value(false)
        .implicit_value(true);

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
        use_uniform = program.get<bool>("--uniform");
        binary = program.get<bool>("--binary");
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
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Parameterization failed: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Output vertices: " << mesh.n_vertices() << std::endl;

    try
    {
        IOFlags flags;
        flags.use_binary = binary;
        write(mesh, output_file, flags);
    }
    catch (const IOException& e)
    {
        std::cerr << "Failed to write mesh: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Saved to: " << output_file << std::endl;
    return 0;
}