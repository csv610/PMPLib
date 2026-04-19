// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/normals.h>
#include <pmp/algorithms/curvature.h>
#include <pmp/algorithms/features.h>
#include <pmp/algorithms/utilities.h>
#include <pmp/algorithms/differential_geometry.h>

#include <iostream>
#include <iomanip>
#include <getopt.h>

using namespace pmp;

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"curvature", no_argument, 0, 'c'},
    {"features", no_argument, 0, 'f'},
    {0, 0, 0, 0}
};

void usage_and_exit()
{
    std::cerr << "Usage: meshinfo [options] <input>\n\n"
              << "Display information about a polygonal mesh.\n\n"
              << "Options:\n"
              << "  -h, --help          show this help message\n"
              << "  -c, --curvature    compute and display curvature information\n"
              << "  -f, --features    compute and display feature edge information\n"
              << "\n"
              << "Mesh information displayed:\n"
              << "  - number of vertices, faces, edges, halfedges\n"
              << "  - bounding box (min/max coordinates)\n"
              << "  - surface area\n"
              << "  - average edge length\n"
              << "  - (with --curvature) mean curvature statistics (min/max/avg)\n"
              << "  - (with --features) number of feature edges at 30 degree angle\n"
              << "\n"
              << "Example:\n"
              << "  meshinfo --curvature --features input.off\n";
    exit(1);
}

int main(int argc, char** argv)
{
    bool compute_curvature = false;
    bool compute_features = false;

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "hcf", long_options, &option_index)) != -1)
    {
        switch (opt)
        {
            case 'h':
                usage_and_exit();
                break;
            case 'c':
                compute_curvature = true;
                break;
            case 'f':
                compute_features = true;
                break;
            default:
                usage_and_exit();
        }
    }

    if (argc - optind < 1)
    {
        usage_and_exit();
    }

    const char* input = argv[optind];

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

    std::cout << "Mesh: " << input << "\n\n";
    std::cout << "  Vertices:       " << mesh.n_vertices() << "\n";
    std::cout << "  Faces:         " << mesh.n_faces() << "\n";
    std::cout << "  Edges:         " << mesh.n_edges() << "\n";
    std::cout << "  Halfedges:    " << mesh.n_halfedges() << "\n";

    auto bb = bounds(mesh);
    std::cout << "\nBounding Box:\n";
    std::cout << "  Min: " << bb.min() << "\n";
    std::cout << "  Max: " << bb.max() << "\n";

    Scalar area = 0.0;
    for (auto f : mesh.faces())
    {
        auto hv = mesh.halfedge(f);
        auto v0 = mesh.to_vertex(hv);
        auto v1 = mesh.to_vertex(mesh.next_halfedge(hv));
        auto v2 = mesh.to_vertex(mesh.next_halfedge(mesh.next_halfedge(hv)));
        auto p0 = mesh.position(v0);
        auto p1 = mesh.position(v1);
        auto p2 = mesh.position(v2);
        area += triangle_area(p0, p1, p2);
    }
    std::cout << "\nSurface Area: " << std::fixed << std::setprecision(6)
              << area << "\n";

    if (mesh.n_vertices() > 0)
    {
        Scalar avg_edge_length = 0.0;
        int edge_count = 0;
        for (auto e : mesh.edges())
        {
            auto p0 = mesh.position(mesh.vertex(e, 0));
            auto p1 = mesh.position(mesh.vertex(e, 1));
            avg_edge_length += distance(p0, p1);
            edge_count++;
        }
        if (edge_count > 0)
        {
            avg_edge_length /= edge_count;
            std::cout << "Avg Edge Length: " << avg_edge_length << "\n";
        }
    }

    if (compute_curvature)
    {
        std::cout << "\nComputing curvature...\n";
        curvature(mesh, Curvature::Mean);
        auto curv_prop = mesh.get_vertex_property<Scalar>("v:curv");
        if (curv_prop)
        {
            Scalar min_curv = 0.0, max_curv = 0.0, avg_curv = 0.0;
            int valid_count = 0;
            for (auto v : mesh.vertices())
            {
                auto curv_val = curv_prop[v];
                if (curv_val == curv_val)
                {
                    min_curv = (valid_count == 0) ? curv_val : std::min(min_curv, curv_val);
                    max_curv = (valid_count == 0) ? curv_val : std::max(max_curv, curv_val);
                    avg_curv += curv_val;
                    valid_count++;
                }
            }
            if (valid_count > 0)
            {
                avg_curv /= valid_count;
                std::cout << "Mean Curvature:\n";
                std::cout << "  Min:  " << min_curv << "\n";
                std::cout << "  Max:  " << max_curv << "\n";
                std::cout << "  Avg:  " << avg_curv << "\n";
            }
        }
    }

    if (compute_features)
    {
        std::cout << "\nComputing features...\n";
        detect_features(mesh, 30.0);
        auto feature_prop = mesh.get_edge_property<bool>("e:feature");
        int feature_count = 0;
        if (feature_prop)
        {
            for (auto e : mesh.edges())
            {
                if (feature_prop[e])
                    feature_count++;
            }
        }
        std::cout << "Feature Edges (30 deg): " << feature_count << "\n";
    }

    return 0;
}