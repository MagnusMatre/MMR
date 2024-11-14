#include "FeatureScript.h"

FeatureScript::FeatureScript(std::string& input_dir, std::string& output_dir, bool save_to_file) {
	m_input_dir = input_dir;
	m_output_dir = output_dir;

	m_save_to_file = save_to_file;
}

FeatureScript::~FeatureScript() {
}

void FeatureScript::Execute() {
}


void FeatureScript::test_mesh(std::string& file_name, std::ofstream& feature_file, std::string& samplepoints_file_name, std::string& convexhull_file_name) {
	// Load the mesh from the file
	std::cout << "Computing mesh: " << file_name << std::endl;

	FeatureExtraction featureExtractor;

	featureExtractor.Load(file_name, convexhull_file_name, samplepoints_file_name);

	featureExtractor.compute_all_features();
	featureExtractor.print_all_features();

	std::cout << "---------------------------------------------------------------" << std::endl;

	if (m_save_to_file)
		feature_file << featureExtractor.get_feature_string() << std::endl;

}

void FeatureScript::test_directory(std::string& directory_name, std::string start_mesh) {
	// Loop through all the files in the directory and process each mesh
	
	// Get directory name
	std::filesystem::path directory_path(directory_name);
	std::string class_name = directory_path.filename().string();

	// Check if the output file already exists, if so skip
	std::string outputFile = directory_path.string() + "/features_large.txt";
	/*if (std::filesystem::exists(outputFile)) {
		std::cout << "File: " << outputFile << " already exists, skipping..." << std::endl;
		return;
	}*/

	// Open a file to save the features to
	std::ofstream feature_file;
	if (m_save_to_file) {
		feature_file.open(directory_name + "/features_large.txt");
		feature_file << "ClassName\t\tFilename\t\tDiameter\tBB_Diameter\tBB_Volume\tSurfaceArea\tVolume\tVolumeComps\tConvexity\tEccentricity02\tEccentricity01\tEccentricity12\tCompactness\tSphericity\tRectangularity";
		feature_file << "\tA3_histogram(100)";
		for (int i = 0; i < 100 - 1; i++) {
			feature_file << "\t ________";
		}
		feature_file << "\tD1_histogram(100)";
		for (int i = 0; i < 100 - 1; i++) {
			feature_file << "\t ________";
		}
		feature_file << "\tD2_histogram(100)";
		for (int i = 0; i < 100 - 1; i++) {
			feature_file << "\t ________";
		}
		feature_file << "\tD3_histogram(100)";
		for (int i = 0; i < 100 - 1; i++) {
			feature_file << "\t ________";
		}
		feature_file << "\tD4_histogram(100)";
		for (int i = 0; i < 100 - 1; i++) {
			feature_file << "\t ________";
		}
		feature_file << std::endl;
	}

	for (const auto& entry : std::filesystem::directory_iterator(directory_name)) {
		// Get the file name
		std::filesystem::path file_path(entry);
		std::string obj_name = file_path.filename().string();

		// If the file is not a file with the .obj extension, skip it
		if (obj_name.find(".obj") == std::string::npos) {
			continue;
		}

		if (obj_name < start_mesh) { // Skip all meshes before the start_mesh
			std::cout << "Skipping mesh: " << obj_name << std::endl;
			continue;
		}

		if (m_save_to_file)
			feature_file << class_name << "\t" << obj_name << "\t";

		std::string mesh_path = entry.path().string();
		std::string convexhull_filename = m_convexhull_root + "/" + class_name + "/" + obj_name;

		obj_name.replace(obj_name.end() - 4, obj_name.end(), ".txt");
		std::string samplepoints_filename = m_samplepoints_root + "/" + class_name + "/" + obj_name;

		test_mesh(mesh_path, feature_file, samplepoints_filename, convexhull_filename);
	}

	if (m_save_to_file)
		feature_file.close();
}

void FeatureScript::load_mesh(std::string& filename, CGALMesh& mesh) {
	if (!CGALPMP::IO::read_polygon_mesh(filename, mesh)) {
		std::cerr << "Error: cannot read file " << filename << std::endl;
		return;
	}

	if (is_empty(mesh))
	{
		std::cerr << "Warning: empty file?" << std::endl;
		return;
	}
}

void FeatureScript::save_sample_points_to_file(std::string& output_name) {
	std::ofstream sample_points_file;
	sample_points_file.open(output_name);

	std::cout << output_name << std::endl;

	for (int i = 0; i < m_featureExtractor.m_random_vertices.size(); i++) {
		Point_3 p = m_featureExtractor.m_random_vertices[i];
		sample_points_file << p.x() << " " << p.y() << " " << p.z() << std::endl;
	}

	sample_points_file.close();
}

void FeatureScript::handle_all_directories(std::string& start_dir) {
	/*
		Compute features for all meshes
	*/

	for (const auto& class_folder : std::filesystem::directory_iterator(m_input_dir)) {
		//Check if the current folder is a directory
		if (!std::filesystem::is_directory(class_folder)) {
			continue;
		}

		std::filesystem::path class_folder_path(class_folder);
		std::string directoryName = class_folder_path.filename().string();

		if (directoryName < start_dir) { // Skip all folders before the start_dir
			std::cout << "Skipping class folder: " << class_folder << std::endl;
			continue;
		}

		std::cout << "Handling class folder: " << class_folder << std::endl;
		std::string class_path = class_folder.path().string();

		// FOR COMPUTING FEATURES
		test_directory(class_path);

		// FOR COMPUTING SAMPLE POINTS
		//for (const auto& entry : std::filesystem::directory_iterator(m_input_dir + "/" + directoryName)) {


		//	std::filesystem::path filePath(entry);
		//	std::string fileName = filePath.filename().string();
		//	std::string outputFile = m_output_dir + "/" + directoryName + "/" + fileName;

		//	// replace the .obj extension with .txt
		//	outputFile.replace(outputFile.end() - 4, outputFile.end(), ".txt");

		//	// Check if the outputFile exists or if the file is in "skip_meshes", if it does, skip the current mesh
		//	if (std::filesystem::exists(outputFile)) {
		//		std::cout << "File: " << outputFile << " already exists, skipping..." << std::endl;
		//		continue;
		//	}

		//	bool is_success = true;

		//	// Load the mesh
		//	std::string input_name = entry.path().string();
		//	//CGALMesh mesh;
		//	//load_mesh(input_name, mesh);

		//	// Check if the number of faces is larger than 0
		//	/*if (mesh.number_of_faces() == 0) {
		//		std::cout << "Error: Mesh has 0 faces, skipping..." << std::endl;
		//		continue;
		//	}*/

		//	std::string hull_file_name = "";
		//	std::string sample_points_file_name = "";


		//	m_featureExtractor.Load(input_name, hull_file_name, sample_points_file_name);
		//	m_featureExtractor.get_N_random_vertices(100000);
		//	save_sample_points_to_file(outputFile);
		//}
	}
}