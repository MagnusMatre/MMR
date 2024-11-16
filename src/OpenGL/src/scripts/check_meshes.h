#pragma once

#include <iostream>

#include "VCGclasses.h" 

#include <wrap/io_trimesh/import.h>

#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>

#include <iostream>
#include <filesystem>
#include <fstream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel KernelExact;
typedef KernelExact::Point_3 ExactPoint_3;
typedef CGAL::Surface_mesh< ExactPoint_3> ExactMesh;

class check_meshes {
public:
	void Execute(std::string& input_dir) {
		check_directories(input_dir);
		
	}

	void CGAL_load_mesh(std::string filename, ExactMesh& mesh) {
		if (!CGALPMP::IO::read_polygon_mesh(filename, mesh)) {
			std::cerr << "Error: cannot read file " << filename << std::endl;
			exit(EXIT_FAILURE);
		}

		if (CGAL::is_empty(mesh))
		{
			std::cerr << "Warning: empty file?" << std::endl;
			return;
		}

		std::cout << "Successfully loaded " << filename << std::endl;
	}

	void is_closed(ExactMesh& mesh) {
		m_closed = 0;
		if (CGAL::is_closed(mesh)) {
			m_closed = 1;
		}


	}

	void check_mesh(MyMesh& mesh, std::ofstream& log_file, std::string& class_name, std::string& obj_name) {
		log_file << class_name << "," << obj_name << ",";
		// Check number of vertices
		log_file << mesh.vn << ",";
		// Check number of faces
		log_file << mesh.fn << ",";
		// Check number of edges
		log_file << mesh.en << ",";
		// Check whether mesh is closed or not
		log_file << m_closed << std::endl;

	}

	void check_directories(std::string& input_dir) {
		// Open a log file
		std::ofstream log_file;
		log_file.open(input_dir + "/mesh_check.txt");
		log_file << "Class,FileName,VertexCount,FaceCount,EdgeCount,Closed" << std::endl;

		ExactMesh CGALmesh;

		for (const auto& class_folder : std::filesystem::directory_iterator(input_dir)) {
			//Check if the current folder is a directory
			if (!std::filesystem::is_directory(class_folder)) {
				continue;
			}

			//Open a log file names "remeshLog.txt" to write the output log to
			std::filesystem::path class_folder_path(class_folder);
			std::string directoryName = class_folder_path.filename().string();

			std::cout << "Handling class folder: " << class_folder << std::endl;

			for (const auto& entry : std::filesystem::directory_iterator(input_dir + "/" + directoryName)) {
				// Check if file has .obj extension
				if (entry.path().extension() != ".obj") {
					continue;
				}

				std::filesystem::path filePath(entry);
				std::string objName = filePath.filename().string();

				// Load the mesh
				MyMesh mesh;
				if (vcg::tri::io::Importer<MyMesh>::Open(mesh, entry.path().string().c_str()) != 0) {
					std::cerr << "Error reading file: " << entry.path().string() << std::endl;
					continue;
				}
				
				CGAL_load_mesh(entry.path().string().c_str(), CGALmesh);
				is_closed(CGALmesh);

				check_mesh(mesh, log_file, directoryName, objName);
			}
		}
	
		log_file.close();
	}
private:
	bool m_closed = 0;
};