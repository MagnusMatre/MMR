#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/IO/OBJ.h>
#include <CGAL/polygon_mesh_processing.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>

#include <CGAL/Polyhedron_3.h>

#include <CGAL/Polygon_mesh_processing/orientation.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup_extension.h>

#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/Polygon_mesh_processing/repair_self_intersections.h>
#include <CGAL/Polygon_mesh_processing/repair_polygon_soup.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Polygon_mesh_processing/border.h>
#include <CGAL/Polygon_mesh_processing/repair_degeneracies.h>
#include <CGAL/Polygon_mesh_processing/self_intersections.h>

#include <CGAL/Polygon_mesh_processing/refine.h>

#include <CGAL/Polygon_mesh_processing/smooth_mesh.h>

#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_count_ratio_stop_predicate.h>

#include <CGAL/subdivision_method_3.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h> // not using this yet

#include <CGAL/bounding_box.h>

#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>

#include <boost/iterator/function_output_iterator.hpp>

#include "VCGclasses.h" 

#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>

#include <vcg/complex/algorithms/local_optimization.h>
#include <vcg/complex/algorithms/edge_collapse.h> // Edge collapse algorithm
#include "vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h"


// VCG type definitions
typedef BasicVertexPair<MyVertex> VertexPair;

class MyTriEdgeCollapse : public vcg::tri::TriEdgeCollapseQuadric< MyMesh, VertexPair, MyTriEdgeCollapse, QInfoStandard<MyVertex>  > {
public:
	typedef  vcg::tri::TriEdgeCollapseQuadric< MyMesh, VertexPair, MyTriEdgeCollapse, QInfoStandard<MyVertex>  > TECQ;
	typedef  MyMesh::VertexType::EdgeType EdgeType;
	inline MyTriEdgeCollapse(const VertexPair& p, int i, BaseParameterClass* pp) :TECQ(p, i, pp) {}
};


#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

// CGAL type definitions
typedef CGAL::Simple_cartesian<double> K;
typedef CGAL::Surface_mesh<K::Point_3> Mesh;
typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point_3;
typedef std::vector<std::size_t> Polygon_3;

typedef CGAL::Exact_predicates_inexact_constructions_kernel KernelExact;
typedef KernelExact::Point_3 ExactPoint_3;
typedef CGAL::Surface_mesh< ExactPoint_3> ExactMesh;

typedef boost::graph_traits<Mesh>::halfedge_descriptor        halfedge_descriptor;
typedef boost::graph_traits<Mesh>::edge_descriptor            edge_descriptor;
typedef boost::graph_traits<Mesh>::face_descriptor            face_descriptor;
typedef boost::graph_traits<Mesh>::vertex_descriptor		  vertex_descriptor;

typedef boost::graph_traits<ExactMesh>::halfedge_descriptor        exact_halfedge_descriptor;
typedef boost::graph_traits<ExactMesh>::edge_descriptor            exact_edge_descriptor;
typedef boost::graph_traits<ExactMesh>::face_descriptor            exact_face_descriptor;
typedef boost::graph_traits<ExactMesh>::vertex_descriptor		   exact_vertex_descriptor;

typedef CGAL::Polyhedron_3<KernelExact> ExactPolyhedron;
typedef ExactPolyhedron::Vertex_handle Vertex_handle;

typedef CGAL::AABB_face_graph_triangle_primitive<Mesh> Primitive;
typedef CGAL::AABB_traits<K, Primitive> AABB_traits;
typedef CGAL::AABB_tree<AABB_traits> Tree;

typedef boost::graph_traits<Mesh>::vertex_descriptor  vertex_descriptor;

namespace CGALPMP = CGAL::Polygon_mesh_processing;
namespace NP = CGAL::parameters;

/*
	Performs all remeshing steps:

	1. Loading the mesh with CGAL
	2. Try to fill holes in the mesh
	3. Doing subdivision if the mesh contains too few faces
	4. Doing decimation on all meshes until they have 4000 vertices
	5. Loading the mesh again with CGAL
	6. Saving the mesh to a new file

*/

class FullRemesh {

public:
	FullRemesh(std::string& mesh_directory, std::string& output_directory, int target);
	~FullRemesh();

	

	void handle_all_directories();

private:

	std::string m_mesh_directory;
	std::string m_output_dir;
	int m_target = 4000;

	void handle_mesh(std::string& file_name, ExactMesh& CGALmesh);
	
	bool load_CGAL_mesh(std::string& file_name, ExactMesh& mesh);
	bool save_CGAL_mesh(std::string& output_name, ExactMesh& mesh);

	void fill_holes(ExactMesh& mesh, double diag);
	void do_subdivision(ExactMesh& mesh);
	void do_decimation(MyMesh& mesh);


	bool is_small_hole(exact_halfedge_descriptor h, ExactMesh& mesh, double max_hole_diam, int max_num_hole_edges);

};
