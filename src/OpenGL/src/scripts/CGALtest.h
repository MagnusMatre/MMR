#pragma once

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/IO/OBJ.h>
#include <CGAL/polygon_mesh_processing.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <iostream>
#include <fstream>

//Include CGAL library to load in mesh

typedef CGAL::Simple_cartesian<double> K;
typedef CGAL::Surface_mesh<K::Point_3> Mesh;
typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point_3;
typedef std::vector<std::size_t> Polygon_3;


namespace CGALPMP = CGAL::Polygon_mesh_processing;

class CGALtest {
public:
	void Execute() {
		Mesh mesh;
		std::string filename = "../../data/VCGremeshedMeshes/TruckNonContainer/D00143.obj";
		std::string output_filename = "../../data/CGALtestMeshes/TruckNonContainer/D00143.obj";

		//load_mesh2(filename, mesh);
		load_mesh(filename, mesh);

		std::cout << "Mesh loaded successfully!" << std::endl;
		std::cout << "Number of vertices: " << num_vertices(mesh) << std::endl;
		std::cout << "Number of faces: " << num_faces(mesh) << std::endl;

		//save_mesh(output_filename, mesh);
	}


	void load_mesh(std::string& filename, Mesh& mesh) {
		std::ifstream input(filename);
		if (!input) {
			std::cerr << "Error: cannot open file " << filename << std::endl;
			exit(EXIT_FAILURE);
		}

		std::vector<Point_3> points;
		std::vector<Polygon_3> faces;

		if (!CGAL::IO::read_OBJ(input, points, faces)) {
			std::cerr << "Error: cannot read file " << filename << std::endl;
			exit(EXIT_FAILURE);
		}

		CGAL::Polygon_mesh_processing::orient_polygon_soup(points, faces);
		CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, faces, mesh);

		std::cout << "Successfully loaded " << filename << std::endl;
		std::cout << "Mesh has " << mesh.number_of_vertices() << " vertices and "
			<< mesh.number_of_faces() << " faces." << std::endl;

	}

	void load_mesh2(std::string& filename, Mesh& mesh) {
		if (!CGALPMP::IO::read_polygon_mesh(filename, mesh)) {
			std::cerr << "Error: cannot read file " << filename << std::endl;
			exit(EXIT_FAILURE);
		}

		if (is_empty(mesh))
		{
			std::cerr << "Warning: empty file?" << std::endl;
			return;
		}

		std::cout << "Successfully loaded " << filename << std::endl;

	}

	void save_mesh(std::string& output_filename, Mesh& mesh) {
		std::ofstream output(output_filename);
		if (!output) {
			std::cerr << "Error: cannot open file " << output_filename << std::endl;
			exit(EXIT_FAILURE);
		}

		if (CGAL::IO::write_polygon_mesh(output_filename, mesh, CGAL::parameters::stream_precision(17))) {
			std::cout << "Successfully saved mesh to " << output_filename << std::endl;
		}
		else {
			std::cerr << "Error: cannot write file " << output_filename << std::endl;
			exit(EXIT_FAILURE);
		}

	}

};