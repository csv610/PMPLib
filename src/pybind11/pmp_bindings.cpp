// Copyright 2024 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <nanobind/nanobind.h>
#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>
#include <pmp/algorithms/decimation.h>
#include <pmp/algorithms/smoothing.h>
#include <pmp/algorithms/subdivision.h>
#include <pmp/algorithms/fairing.h>
#include <pmp/algorithms/remeshing.h>
#include <pmp/algorithms/hole_filling.h>
#include <pmp/algorithms/parameterization.h>
#include <pmp/algorithms/normals.h>
#include <pmp/algorithms/curvature.h>
#include <pmp/algorithms/features.h>
#include <pmp/algorithms/utilities.h>

namespace nb = nanobind;

using namespace pmp;

NB_MODULE(pmplib, m)
{
    m.doc() = "PMPLib - Polygon Mesh Processing Library";

    nb::class_<SurfaceMesh>(m, "SurfaceMesh")
        .def(nb::init<>())
        .def("n_vertices", &SurfaceMesh::n_vertices, "Returns the number of vertices")
        .def("n_faces", &SurfaceMesh::n_faces, "Returns the number of faces")
        .def("n_edges", &SurfaceMesh::n_edges, "Returns the number of edges")
        .def("read", [](SurfaceMesh& mesh, const std::string& filename) {
            read(mesh, filename);
        }, "Read mesh from file")
        .def("write", [](const SurfaceMesh& mesh, const std::string& filename) {
            write(mesh, filename);
        }, "Write mesh to file");

    m.def("decimate", [](SurfaceMesh& mesh, unsigned int n_vertices,
                       Scalar aspect_ratio = 0.0, Scalar edge_length = 0.0,
                       unsigned int max_valence = 0, Scalar normal_deviation = 0.0,
                       Scalar hausdorff_error = 0.0) {
        decimate(mesh, n_vertices, aspect_ratio, edge_length, max_valence,
                normal_deviation, hausdorff_error);
    }, "Simplify mesh using quadric-based decimation", nb::arg("mesh"),
       nb::arg("n_vertices"), nb::arg("aspect_ratio") = 0.0, nb::arg("edge_length") = 0.0,
       nb::arg("max_valence") = 0, nb::arg("normal_deviation") = 0.0, nb::arg("hausdorff_error") = 0.0);

    m.def("explicit_smoothing", [](SurfaceMesh& mesh, unsigned int iterations,
                                  bool use_uniform = false) {
        explicit_smoothing(mesh, iterations, use_uniform);
    }, "Apply explicit Laplacian smoothing", nb::arg("mesh"),
       nb::arg("iterations") = 10, nb::arg("use_uniform") = false);

    m.def("implicit_smoothing", [](SurfaceMesh& mesh, Scalar timestep,
                                 unsigned int iterations, bool use_uniform = false,
                                 bool rescale = true) {
        implicit_smoothing(mesh, timestep, iterations, use_uniform, rescale);
    }, "Apply implicit Laplacian smoothing", nb::arg("mesh"), nb::arg("timestep"),
       nb::arg("iterations") = 10, nb::arg("use_uniform") = false, nb::arg("rescale") = true);

    m.def("catmull_clark_subdivision", [](SurfaceMesh& mesh) {
        catmull_clark_subdivision(mesh);
    }, "Catmull-Clark subdivision");

    m.def("loop_subdivision", [](SurfaceMesh& mesh) {
        loop_subdivision(mesh);
    }, "Loop subdivision");

    m.def("quad_tri_subdivision", [](SurfaceMesh& mesh) {
        quad_tri_subdivision(mesh);
    }, "Quad-triangle subdivision");

    m.def("linear_subdivision", [](SurfaceMesh& mesh) {
        linear_subdivision(mesh);
    }, "Linear subdivision");

    m.def("minimize_curvature", [](SurfaceMesh& mesh) {
        minimize_curvature(mesh);
    }, "Minimize curvature");

    m.def("minimize_area", [](SurfaceMesh& mesh) {
        minimize_area(mesh);
    }, "Minimize surface area");

    m.def("fair", [](SurfaceMesh& mesh, unsigned int order = 2) {
        fair(mesh, order);
    }, "Fair mesh using k-harmonic optimization", nb::arg("mesh"),
       nb::arg("order") = 2);

    m.def("uniform_remeshing", [](SurfaceMesh& mesh, Scalar edge_length,
                                unsigned int iterations = 10,
                                bool use_projection = true) {
        uniform_remeshing(mesh, edge_length, iterations, use_projection);
    }, "Uniform remeshing", nb::arg("mesh"), nb::arg("edge_length"),
       nb::arg("iterations") = 10, nb::arg("use_projection") = true);

    m.def("adaptive_remeshing", [](SurfaceMesh& mesh, Scalar min_edge_length,
                                  Scalar max_edge_length, Scalar approx_error,
                                  unsigned int iterations = 10,
                                  bool use_projection = true) {
        adaptive_remeshing(mesh, min_edge_length, max_edge_length, approx_error,
                          iterations, use_projection);
    }, "Adaptive remeshing", nb::arg("mesh"), nb::arg("min_edge_length"),
       nb::arg("max_edge_length"), nb::arg("approx_error"), nb::arg("iterations") = 10,
       nb::arg("use_projection") = true);

    m.def("fill_hole", [](SurfaceMesh& mesh, Halfedge halfedge) {
        fill_hole(mesh, halfedge);
    }, "Fill a hole", nb::arg("mesh"), nb::arg("halfedge"));

    m.def("harmonic_parameterization", [](SurfaceMesh& mesh, bool use_uniform = false) {
        harmonic_parameterization(mesh, use_uniform);
    }, "Compute harmonic parameterization", nb::arg("mesh"),
       nb::arg("use_uniform") = false);

    m.def("lscm_parameterization", [](SurfaceMesh& mesh) {
        lscm_parameterization(mesh);
    }, "Compute LSCM parameterization");

    m.def("vertex_normals", [](SurfaceMesh& mesh) {
        vertex_normals(mesh);
    }, "Compute vertex normals");

    m.def("face_normals", [](SurfaceMesh& mesh) {
        face_normals(mesh);
    }, "Compute face normals");

    m.def("curvature", [](SurfaceMesh& mesh) {
        curvature(mesh, Curvature::Mean);
    }, "Compute curvature");

    m.def("detect_features", [](SurfaceMesh& mesh, Scalar angle = 30.0) {
        detect_features(mesh, angle);
    }, "Detect features", nb::arg("mesh"), nb::arg("angle") = 30.0);
}