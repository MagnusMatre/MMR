#pragma once

#include <iostream>
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

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>


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

struct halfedge2edge
{
	halfedge2edge(const Mesh& m, std::vector<edge_descriptor>& edges): m_mesh(m), m_edges(edges) {}

	void operator()(const halfedge_descriptor& h) const
	{
		m_edges.push_back(CGAL::edge(h, m_mesh));
	}
	const Mesh& m_mesh;
	std::vector<edge_descriptor>& m_edges;
};

class CGALclean {
public:
	CGALclean(std::string& input_dir, std::string& ouput_dir, int target, int threshold);

	void Execute();

	void handle_all_directories(std::string& directory);

private:
	std::string m_input_dir;
	std::string m_output_dir;

	int m_target;
	int m_threshold;

	int m_max_remesh_iterations = 10; // Sometimes used to just populate 10 meshes for each class

	//void polygon_soup_repair(std::vector<Point_3>& points, std::vector<Polygon_3>& faces);
	//void fix_manifoldedness(Mesh& mesh);
	//void repair_and_remesh(std::vector<Point_3>& points, std::vector<Polygon_3>& faces, Mesh& mesh, bool& is_success);

	void smooth(ExactMesh& mesh);
	void refine(ExactPolyhedron& mesh);
	void remesh(Mesh& mesh);
	void simple_subdivide(ExactMesh& mesh);

	void fill_holes(ExactMesh& mesh, double diag);

	bool is_small_hole(exact_halfedge_descriptor h, ExactMesh& mesh, double max_hole_diam, int max_num_hole_edges);
	void orient_mesh(ExactMesh& mesh);

	//double getAABoundingBoxDiag(std::vector<Point_3> points);
	double getAABoundingBoxDiag(Mesh& mesh);

	void save_mesh(std::string& output_filename, Mesh& mesh);
	void load_mesh(std::string& filename, Mesh& mesh); 
	void save_mesh(std::string& output_filename, ExactMesh& mesh);
	void load_mesh(std::string& filename, ExactMesh& mesh);
	void save_mesh(std::string& output_filename, ExactPolyhedron& mesh);
	void load_mesh(std::string& filename, ExactPolyhedron& mesh);
	//void load_soup(std::string& filename, std::vector<Point_3>& points, std::vector<Polygon_3>& faces);
};