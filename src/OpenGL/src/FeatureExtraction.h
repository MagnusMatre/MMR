#pragma once

// Ideally number of bins should be around sqrt(N) where N is the number of samples
#define NUM_A3_BINS 60 // sqrt(10800/3) = 60
#define NUM_D1_BINS 104 // sqrt(10800) = 104
#define NUM_D2_BINS 74 // sqrt(10800/2) = 74
#define NUM_D3_BINS 60 // sqrt(10800/3) = 60
#define NUM_D4_BINS 52 // sqrt(10800/4) = 52

#define NUM_CURV_BINS 10

#define RBOUND_A3 3.14159265360
#define RBOUND_D1 1.73205080757 // Note: maximal distance to barycenter is sqrt(3) since mesh is inside cube [-1,1]^3
#define RBOUND_D2 3.46410161514 // Note: maximal distance between two points is 2sqrt(3) since mesh is inside cube [-1,1]^3
#define RBOUND_D3 1.86120971820 // Note: maximal area of triangle is 2sqrt(3) since mesh is inside cube [-1,1]^3 (and then sqrt)
#define RBOUND_D4 1.38672254871 // Note: maximal volume of tetrahedron is 8/3 since mesh is inside cube [-1,1]^3 (and then cube root)

#include <iostream>

#include <iostream>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>

#include <CGAL/polygon_mesh_processing.h>

// For computing area and volume
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <CGAL/Polygon_mesh_processing/connected_components.h>

// For computing convex hull (CGAL attempt)
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>

// For computing conve hull (QHull)
#include <libqhullcpp/Qhull.h>

// For computing bounding box
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>

// For computing eccentricity
#include <Eigen/Dense>

#include <random>

// This class is responsible for extracting features from a given mesh. The mesh is assumed to be a CGAL Surface_mesh.

typedef CGAL::Simple_cartesian<double> K;
typedef CGAL::Surface_mesh<K::Point_3> CGALMesh;
typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point_3;
typedef Kernel::Vector_3 Vector_3;
typedef CGAL::Polyhedron_3<K> Polyhedron_3;
typedef std::vector<std::size_t> Polygon_3;

typedef CGAL::AABB_face_graph_triangle_primitive<CGALMesh> Primitive;
typedef CGAL::AABB_traits<K, Primitive> AABB_traits;
typedef CGAL::AABB_tree<AABB_traits> Tree;

typedef boost::graph_traits<CGALMesh>::face_descriptor face_descriptor;

namespace CGALPMP = CGAL::Polygon_mesh_processing;

class FeatureExtraction {
public:
	FeatureExtraction();
	~FeatureExtraction();

	void Load(std::string& mesh_path, std::string& convex_hull_path, std::string& samplepoints_file_name);
	void compute_all_features();
	void print_all_features();
	std::string get_feature_string();
	
	// histogram features
	void get_N_random_vertices(int N);
	std::vector<Point_3> m_random_vertices;

	// convex hulls
	void compute_convex_hull();
private:

	CGALMesh m_mesh;
	CGALMesh m_convexhull;

	// Final feature values
	double m_surface_area = -1.0f;
	double m_diameter = -1.0f;
	double m_volume = -1.0f; // naive volume
	double m_volume2 = -1.0f; // volume of connected components
	double m_bounding_box_diameter = -1.0f;
	double m_bounding_box_volume = -1.0f;
	double m_hull_volume = -1.0f;
	double m_hull_surface_area = -1.0f;
	double m_convexity = -1.0f;
	double m_eccentricity02 = -1.0f;
	double m_eccentricity01 = -1.0f;
	double m_eccentricity12 = -1.0f;
	double m_compactness = -1.0f;
	double m_rectangularity = -1.0f;
	double m_sphericity = -1.0f;

	int m_num_comps = -1;

	// Handle feature computation
	void get_surface_area();
	void get_volume();
	void get_concomp_volume();
	void get_bounding_box_statistics();
	void get_diameter();
	void get_convexity();
	void get_eccentricities();
	void get_compactness();
	void get_rectangularity();
	void get_sphericity();

	//histogram features
	float A3_bins[NUM_A3_BINS] = { 0.0f }; 
	float D1_bins[NUM_D1_BINS] = { 0.0f }; 
	float D2_bins[NUM_D2_BINS] = { 0.0f }; 
	float D3_bins[NUM_D3_BINS] = { 0.0f }; 
	float D4_bins[NUM_D4_BINS] = { 0.0f };
	float CURV_bins[NUM_CURV_BINS] = { 0.0f };

	// TODO: efficiently obtain all features by looping over the sample points a minimal number of times
	void get_A3_histogram(); // Angle between three points
	void get_D1_histogram(); // Distance of a point to barycenter
	void get_D2_histogram(); // Distance between two random points
	void get_D3_histogram(); // Square root are of triangle defined by three random points
	void get_D4_histogram(); // Cube root of volume of tetrahedron defined by four random points

	void load_samplepoints(std::string& filename);
	int get_bin(double val, int N, double lboud, double rbound);

	void load_mesh(std::string& filename, CGALMesh& mesh);

};