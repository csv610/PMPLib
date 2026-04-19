// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/remeshing.h>

#include <argparse/argparse.hpp>

#include <iostream>

using namespace pmp;

int main(int argc, char** argv)
{
    argparse::ArgumentParser program("meshremesh", "1.0", argparse::default_arguments::help);

    std::string input_file;
    std::string output_file;
    std::string mode = "adaptive";
    Scalar edge_length = 0.0;
    Scalar min_edge_length = 0.0;
    Scalar max_edge_length = 0.0;
    Scalar approx_error = 0.0;
    unsigned int iterations = 10;
    bool use_projection = true;
    bool binary = false;

    program.add_argument("-i", "--input")
        .help("Input mesh file")
        .required();

    program.add_argument("-o", "--output")
        .help("Output mesh file")
        .required();

    program.add_argument("-m", "--mode")
        .help("Mode: uniform, adaptive")
        .default_value("adaptive");

    program.add_argument("-l", "--length")
        .help("Target edge length for uniform")
        .default_value(0.0)
        .scan<'g', Scalar>();

    program.add_argument("-n", "--min-length")
        .help("Min edge length for adaptive")
        .default_value(0.0)
        .scan<'g', Scalar>();

    program.add_argument("-x", "--max-length")
        .help("Max edge length for adaptive")
        .default_value(0.0)
        .scan<'g', Scalar>();

    program.add_argument("-e", "--error")
        .help("Approximation error for adaptive")
        .default_value(0.0)
        .scan<'g', Scalar>();

    program.add_argument("-r", "--iterations")
        .help("Number of iterations")
        .default_value(10)
        .scan<'i', unsigned int>();

    program.add_argument("-p", "--no-projection")
        .help("Disable projection to original")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-b", "--binary")
        .help("Write binary format")
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

    input_file = program.get<std::string>("--input");
    output_file = program.get<std::string>("--output");
    mode = program.get<std::string>("--mode");
    edge_length = program.get<Scalar>("--length");
    min_edge_length = program.get<Scalar>("--min-length");
    max_edge_length = program.get<Scalar>("--max-length");
    approx_error = program.get<Scalar>("--error");
    iterations = program.get<unsigned int>("--iterations");
    use_projection = !program.get<bool>("--no-projection");
    binary = program.get<bool>("--binary");

    if (mode == "uniform" && edge_length <= 0.0)
    {
        std::cerr << "Error: --length required for uniform remeshing\n";
        return 1;
    }

    if (mode == "adaptive" && (min_edge_length <= 0.0 || max_edge_length <= 0.0))
    {
        std::cerr << "Error: --min-length and --max-length required for adaptive\n";
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
            std::cout << "Min edge: " << min_edge_length << ", Max edge: " << max_edge_length
                      << ", Approx error: " << approx_error << "\n";
            adaptive_remeshing(mesh, min_edge_length, max_edge_length, approx_error, iterations, use_projection);
        }
        else
        {
            std::cerr << "Unknown mode: " << mode << "\n";
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Remeshing failed: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Output vertices: " << mesh.n_vertices() << "\n";
    std::cout << "Faces: " << mesh.n_faces() << std::endl;

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