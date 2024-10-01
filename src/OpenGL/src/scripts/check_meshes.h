#pragma once

#include <iostream>

#include "VCGclasses.h" 

#include <wrap/io_trimesh/import.h>

#include <iostream>
#include <filesystem>
#include <fstream>

class check_meshes {
public:
	void Execute(std::string& input_dir) {
		check_directories(input_dir);
		
	}


	void check_mesh(MyMesh& mesh, std::ofstream& log_file, std::string& class_name, std::string& obj_name) {
		log_file << class_name << "," << obj_name << ",";
		// Check number of vertices
		log_file << mesh.vn << ",";
		// Check number of faces
		log_file << mesh.fn << ",";
		// Check number of edges
		log_file << mesh.en << std::endl;

	}

	void check_directories(std::string& input_dir) {
		// Open a log file
		std::ofstream log_file;
		log_file.open(input_dir + "/mesh_check.txt");
		log_file << "Class,FileName,VertexCount,FaceCount,EdgeCount" << std::endl;

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
				
				check_mesh(mesh, log_file, directoryName, objName);
			}
		}
	
		log_file.close();
	}
};