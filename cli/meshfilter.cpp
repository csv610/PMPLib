// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/smoothing.h>

#include <argparse/argparse.hpp>

#include <iostream>

using namespace pmp;

int main(int argc, char** argv)
{
    argparse::ArgumentParser program("meshfilter", "1.0", argparse::default_arguments::help);

    std::string input_file;
    std::string output_file;
    unsigned int iterations = 10;
    float timestep = 0.001f;
    bool use_uniform = false;
    bool use_implicit = false;
    bool rescale = true;

    program.add_argument("-i", "--input")
        .help("Input mesh file")
        .required();

    program.add_argument("-o", "--output")
        .help("Output mesh file")
        .required();

    program.add_argument("-n", "--iterations")
        .help("Number of smoothing iterations")
        .default_value(10)
        .scan<'i', unsigned int>();

    program.add_argument("-t", "--timestep")
        .help("Timestep for implicit smoothing")
        .default_value(0.001f)
        .scan<'g', float>();

    program.add_argument("-u", "--uniform")
        .help("Use uniform Laplacian weights")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-s", "--implicit")
        .help("Use implicit smoothing")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-r", "--no-rescale")
        .help("Do not rescale after smoothing")
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
    iterations = program.get<unsigned int>("--iterations");
    timestep = program.get<float>("--timestep");
    use_uniform = program.get<bool>("--uniform");
    use_implicit = program.get<bool>("--implicit");
    rescale = !program.get<bool>("--no-rescale");

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

    if (use_implicit)
    {
        std::cout << "Running implicit smoothing: " << iterations
                  << " iters, timestep=" << timestep
                  << ", uniform=" << use_uniform << "..." << std::endl;
        implicit_smoothing(mesh, timestep, iterations, use_uniform, rescale);
    }
    else
    {
        std::cout << "Running explicit smoothing: " << iterations
                  << " iters, uniform=" << use_uniform << "..." << std::endl;
        explicit_smoothing(mesh, iterations, use_uniform);
    }

    std::cout << "Output vertices: " << mesh.n_vertices() << "\n";

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