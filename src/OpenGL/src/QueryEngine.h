#pragma once

/*
This class is used for querying the databbase.
So it can compute distances between the feature vectors of the meshes.
*/

#define NUM_FEATURES 363 // Length of the feature vector (excluding the class name and the object name)
#define NUM_SHAPES 2285 // Number of shapes in the database

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <fstream>

class QueryEngine {
public:
	QueryEngine();
	~QueryEngine();

	void LoadFeatures(std::string& feature_file);
	float ComputeDistance(std::string& query_obj, std::string& other_obj);

	void ComputeFullDistanceMatrix();
	void SaveDistanceMatrix(std::string& save_path);
	void LoadDistanceMatrix(std::string& load_path);

	int getIndex(std::string file_name);

	//float m_distances[NUM_SHAPES][NUM_SHAPES]; // Store the distances as 2D array of floats ( I think this is too large to store in memory)
	std::vector<std::string> m_name_map;

private:
	float m_features[NUM_SHAPES][NUM_FEATURES]; // Store the features as 2D array of floats
	//std::unordered_map<std::string, int> m_name_map; // Map the shape index to the shape name (e.g. AircraftBuoyant/m1339.obj)
	std::vector<std::string> m_class_map; // Map the shape index to the shape name

	void parseFeatures();


};


