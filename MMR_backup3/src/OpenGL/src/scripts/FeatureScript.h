#pragma once


#include <iostream>
#include <filesystem>
#include <fstream>

#include "FeatureExtraction.h"

class FeatureScript {
public:
	FeatureScript(std::string& input_dir, std::string& output_dir, bool save_to_file, bool hull_and_samplepoints);
	~FeatureScript();

	void Execute();

	//Testing functions...
	void test_mesh(std::string& file_name, std::ofstream& feature_file, std::string& samplepoints_file_name, std::string& convexhull_file_name);
	void test_directory(std::string& directory_name, std::string startMesh = "");
	void handle_all_directories(std::string& start_dir); // Loop through all directories and process each mesh...

private:

	FeatureExtraction m_featureExtractor;

	bool m_save_to_file;
	bool m_hull_and_samplepoints = false;

	std::string m_input_dir;
	std::string m_output_dir;

	std::string m_samplepoints_root = "../../res/sample_points_normalized_meshes";
	std::string m_convexhull_root = "../../res/convex_hulls_normalized_meshes";
	
	//utility functions
	void load_mesh(std::string& filename, CGALMesh& mesh);
	void save_features_to_file();

	void save_sample_points_to_file(std::string& outputname);
};