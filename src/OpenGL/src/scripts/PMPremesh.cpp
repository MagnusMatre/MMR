#include "PMPremesh.h"

#include <fstream>

PMPremesh::PMPremesh(std::string& dir_name, std::string& remesh_dir_name, unsigned int target, unsigned int threshold) {
	m_dir_name = dir_name;
	m_remesh_dir_name = remesh_dir_name;
	m_target = target;
	m_threshold = threshold;
}

PMPremesh::~PMPremesh() {};

void PMPremesh::Execute() {
	//handle_directory();
	//handle_all_meshes();
}

void PMPremesh::handle_all_meshes(std::string& start_dir) {
	// Iterate over all .obj files in the directory and remesh the meshes inside it until it has the target number of vertices

	for (const auto& class_folder : std::filesystem::directory_iterator(m_dir_name)){
		//Check if the current folder is a directory
		if (!std::filesystem::is_directory(class_folder)) {
			continue;
		}

		//Open a log file names "remeshLog.txt" to write the output log to
		std::filesystem::path class_folder_path(class_folder);
		std::string directoryName = class_folder_path.filename().string();

		if (directoryName < start_dir) { // Skip all folders before the start_dir
			std::cout << "Skipping class folder: " << class_folder << std::endl;
			continue;
		}

		std::ofstream logFile;
		logFile.open(m_remesh_dir_name + "/" + directoryName + "/remeshLog.txt");

		std::cout << "Handling class folder: " << class_folder << std::endl;

		for (const auto& entry : std::filesystem::directory_iterator(m_dir_name + "/" + directoryName)) {
			std::filesystem::path filePath(entry);
			std::string fileName = filePath.filename().string();
			std::string outputFile = m_remesh_dir_name + "/" + directoryName + "/" + fileName;

			// Check if the outputFile exists or if the file is in "skip_meshes", if it does, skip the current mesh
			if (std::filesystem::exists(outputFile) || m_skip_meshes.find(directoryName + "/" + fileName) != m_skip_meshes.end()) {
				std::cout << "File: " << outputFile << " already exists, skipping..." << std::endl;
				logFile << "File: " << outputFile << " already exists, skipping..." << std::endl;
				continue;
			}

			// Load the mesh
			pmp::SurfaceMesh mesh;
			try {
				pmp::read(mesh, entry.path().string());
			}
			catch (const std::exception& e) {
				std::cout << "Error reading file: " << entry.path().string() << " because: " << e.what() << std::endl;
				logFile << "Error reading file: " << entry.path().string() << " because: " << e.what() << std::endl;
				continue;
			}
			std::cout << "Read mesh " << entry.path().string() << " successfully" << std::endl;
			logFile << "Read mesh " << entry.path().string() << " successfully" << std::endl;

			// Remesh the mesh
			try {
				remesh_mesh(mesh);
			}
			catch (const std::exception& e) {
				std::cout << "Error remeshing file: " << entry.path().string() << " because: " << e.what() << std::endl;
				logFile << "Error remeshing file: " << entry.path().string() << " because: " << e.what() << std::endl;
				continue;
			}

			std::cout << "Saving mesh to file: " << " " << directoryName << "/" << fileName << " with resulting number of vertices: " << mesh.n_vertices() << std::endl;
			logFile << "Saving mesh to file: " << " " << directoryName << "/" << fileName << " with resulting number of vertices: " << mesh.n_vertices() << std::endl;

			pmp::write(mesh, outputFile);
		}

		logFile.close();
	}

	
}

void PMPremesh::handle_mesh(std::string& file_name) {
	std::cout << "Handling mesh: " << m_dir_name + "/" << file_name << std::endl;

	// Load the mesh
	pmp::SurfaceMesh mesh;
	pmp::read(mesh, m_dir_name + "/" + file_name);

	std::cout << "Reading sucessfully" << std::endl;

	// Remesh the mesh
	remesh_mesh(mesh);

	// Write the result mesh to a file
	std::filesystem::path filePath(file_name);
	std::string fileName = filePath.filename().string();
	std::string directoryName = filePath.parent_path().filename().string();

	std::cout << "Saving mesh to file: " << " " << directoryName << "/" << fileName << " with resulting number of vertices: " << mesh.n_vertices() << std::endl;

	std::string outputFile = m_remesh_dir_name + "/" + directoryName + "/" + fileName;

	pmp::write(mesh, outputFile);
}

void PMPremesh::handle_directory() {
	// Iterate over all .obj files in the directory and remesh the meshes inside it until it has the target number of vertices
	
	//Open a log file names "remeshLog.txt" to write the output log to
	std::filesystem::path filePath(m_dir_name);
	std::string directoryName = filePath.filename().string();
	std::ofstream logFile;
	logFile.open(m_remesh_dir_name + "/" + directoryName + "/remeshLog.txt");

	for (const auto& entry : std::filesystem::directory_iterator(m_dir_name)) {
		// Load the mesh
		pmp::SurfaceMesh mesh;
		try {
			pmp::read(mesh, entry.path().string());
		}
		catch (const std::exception& e) {
			std::cout << "Error reading file: " << entry.path().string() << " because: " << e.what() << std::endl;
			logFile << "Error reading file: " << entry.path().string() << " because: " << e.what() << std::endl;
			continue;
		}
		std::cout << "Read mesh " << entry.path().string() << " successfully" << std::endl;
		logFile << "Read mesh " << entry.path().string() << " successfully" << std::endl;

		// Remesh the mesh
		try {
			remesh_mesh(mesh);
		}
		catch (const std::exception& e) {
			std::cout << "Error remeshing file: " << entry.path().string() << " because: " << e.what() << std::endl;
			logFile << "Error remeshing file: " << entry.path().string() << " because: " << e.what() << std::endl;
			continue;
		}

		// Write the result mesh to a file
		std::filesystem::path filePath(entry);
		std::string fileName = filePath.filename().string();
		std::string directoryName = filePath.parent_path().filename().string();
		std::cout << "Saving mesh to file: " << " " << directoryName << "/" << fileName << " with resulting number of vertices: " << mesh.n_vertices() << std::endl;
		logFile << "Saving mesh to file: " << " " << directoryName << "/" << fileName << " with resulting number of vertices: " << mesh.n_vertices() << std::endl;

		std::string outputFile = m_remesh_dir_name + "/" + directoryName + "/" + fileName;
		try {
			pmp::write(mesh, outputFile);
		}
		catch (const std::exception& e) {
			std::cout << "Error writing file: " << outputFile << " because: " << e.what() << std::endl;
			logFile << "Error writing file: " << outputFile << " because: " << e.what() << std::endl;
			continue;
		}
	}

	logFile.close();
}

void PMPremesh::remesh_mesh(pmp::SurfaceMesh& mesh) {
	// Count number of vertices of the mesh
	unsigned int nv = mesh.n_vertices();
	std::cout << "Number of vertices: " << nv << std::endl;

	// If number of vertices is greater than target, decimate the mesh until the number of vertices is equal to target
	if (nv > m_target + m_threshold) {
		decimate_mesh(mesh);
	}

	//// If number of vertices is less than target, refine the mesh until the number of vertices is within the threshold bounds.
	//else if (nv < m_target - m_threshold) {
	//	refine_mesh(mesh);
	//}
}

void PMPremesh::decimate_mesh(pmp::SurfaceMesh& mesh) {
	std::cout << "Doing a decimation... " << std::endl;
	pmp::decimate(mesh, m_target);
}

void PMPremesh::refine_mesh(pmp::SurfaceMesh& mesh) {
	// Get the number of vertices of the mesh
	unsigned int nv = mesh.n_vertices();

	while (nv < m_target) {
		// Get the average length of the edges of the mesh
		double avg_edge_length = pmp::mean_edge_length(mesh);
		//double alpha = (m_target + m_threshold) / nv;

		std::cout << "Doing a refine loop starting at " << nv << " vertices with average edge length " << avg_edge_length << std::endl;
		pmp::uniform_remeshing(mesh, 0.1 * avg_edge_length, 10); // if set the parameter too low, the refining process can get stuck.

		// Update the number of vertices of the mesh
		nv = mesh.n_vertices();
	}

	if (nv > m_target + m_threshold) {
		decimate_mesh(mesh);
	}
}