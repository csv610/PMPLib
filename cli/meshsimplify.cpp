// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/decimation.h>

#include <argparse/argparse.hpp>

#include <iostream>
#include <sstream>

using namespace pmp;

double parse_double(const std::string& s)
{
    double value = 0.0;
    std::istringstream iss(s);
    iss >> value;
    return value;
}

unsigned int parse_uint(const std::string& s)
{
    unsigned int value = 0;
    std::istringstream iss(s);
    iss >> value;
    return value;
}

int main(int argc, char** argv)
{
    argparse::ArgumentParser program("meshsimplify", "1.0");

    std::string input_file;
    std::string output_file;
    unsigned int n_vertices = 0;
    double aspect_ratio = 0.0;
    double edge_length = 0.0;
    unsigned int max_valence = 0;
    double normal_deviation = 0.0;
    double hausdorff_error = 0.0;

    program.add_argument("-i", "--input")
        .help("Input mesh file")
        .required();

    program.add_argument("-o", "--output")
        .help("Output mesh file")
        .required();

    program.add_argument("-n", "--vertices")
        .help("Target number of vertices")
        .required();

    program.add_argument("-a", "--aspect-ratio")
        .help("Minimum aspect ratio [0-1]")
        .default_value("0.0");

    program.add_argument("-e", "--edge-length")
        .help("Minimum edge length")
        .default_value("0.0");

    program.add_argument("-m", "--max-valence")
        .help("Maximum vertex valence")
        .default_value("0");

    program.add_argument("-d", "--normal-deviation")
        .help("Maximum normal deviation (degrees)")
        .default_value("0.0");

    program.add_argument("-H", "--hausdorff")
        .help("Maximum Hausdorff error")
        .default_value("0.0");

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
        n_vertices = parse_uint(program.get<std::string>("--vertices"));
        aspect_ratio = parse_double(program.get<std::string>("--aspect-ratio"));
        edge_length = parse_double(program.get<std::string>("--edge-length"));
        max_valence = parse_uint(program.get<std::string>("--max-valence"));
        normal_deviation = parse_double(program.get<std::string>("--normal-deviation"));
        hausdorff_error = parse_double(program.get<std::string>("--hausdorff"));
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

    unsigned int orig_verts = mesh.n_vertices();
    std::cout << "Input: " << input_file << "\n";
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
        return 1;
    }

    unsigned int new_verts = mesh.n_vertices();
    std::cout << "Output vertices: " << new_verts
             << " (" << (100.0 * new_verts / orig_verts) << "%)\n";

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