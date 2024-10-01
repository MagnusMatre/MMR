#pragma once

#include <iostream>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/IO/OBJ.h>
#include <CGAL/polygon_mesh_processing.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>

#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/Polygon_mesh_processing/repair_polygon_soup.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Polygon_mesh_processing/border.h>

#include <CGAL/bounding_box.h>

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

typedef boost::graph_traits<Mesh>::halfedge_descriptor        halfedge_descriptor;
typedef boost::graph_traits<Mesh>::edge_descriptor            edge_descriptor;

namespace CGALPMP = CGAL::Polygon_mesh_processing;
namespace NP = CGAL::parameters;

typedef boost::graph_traits<Mesh>::vertex_descriptor  vertex_descriptor;


struct halfedge2edge
{
	halfedge2edge(const Mesh& m, std::vector<edge_descriptor>& edges)
		: m_mesh(m), m_edges(edges)
	{}
	void operator()(const halfedge_descriptor& h) const
	{
		m_edges.push_back(edge(h, m_mesh));
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

	int m_max_remesh_iterations = 10;

	void polygon_soup_repair(std::vector<Point_3>& points, std::vector<Polygon_3>& faces);
	void fix_manifoldedness(Mesh& mesh);
	void repair_and_remesh(std::vector<Point_3>& points, std::vector<Polygon_3>& faces, Mesh& mesh);

	double getAABoundingBoxDiag(std::vector<Point_3> points);

	void save_mesh(std::string& output_filename, Mesh& mesh);
	void load_mesh(std::string& filename, Mesh& mesh); 
	void load_soup(std::string& filename, std::vector<Point_3>& points, std::vector<Polygon_3>& faces);
};