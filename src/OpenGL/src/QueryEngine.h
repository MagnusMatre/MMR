#pragma once

/*
THIS CLASS IS USED FOR QUERYING THE DATABBASE.
SO IT CAN COMPUTE DISTANCES BETWEEN THE FEATURE VECTORS OF THE MESHES.
*/

#define NUM_FEATURES 363 // LENGTH OF THE FEATURE VECTOR (EXCLUDING THE CLASS NAME AND THE OBJECT NAME)
#define NUM_SHAPES 2285 // NUMBER OF SHAPES IN THE DATABASE
#define NUM_SCALAR_FEATURES 13 // NUMBER OF SCALAR FEATURES
#define NUM_HISTOGRAM_FEATURES 5 // NUMBER OF HISTOGRAM FEATURES

#define A3_START 13 // Length 60, 13-72
#define D1_START 73 // Length 104, 73-176
#define D2_START 177 // Length 74, 177-250
#define D3_START 251 // Length 60, 251-310
#define D4_START 311 // Length 52, 311-362

enum SCALAR_FEATURES {
	DIAMETER,
	BB_DIAMETER,
	BB_VOLUME,
	SURFACEAREA,
	VOLUME,
	VOLUMECOMPS,
	CONVEXITY,
	ECCENTRICITY02,
	ECCENTRICITY01,
	ECCENTRICITY12,
	COMPACTNESS,
	SPHERICITY,
	RECTANGULARITY
};

enum HISTOGRAM_FEATURES {
	A1,
	D1,
	D2,
	D3,
	D4
};


#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <fstream>

#include "emd/WassersteinEMD.h"

class QueryEngine {
public:
	QueryEngine();
	~QueryEngine();

	void LoadFeatures(std::string& feature_file);
	float ComputeDistance(std::string& query_obj, std::string& other_obj);

	void ComputeFullDistanceMatrix();
	void SaveDistanceMatrix(std::string& save_path);
	void LoadDistanceMatrix(std::string& load_path);

	void TuneScalarWeights(SCALAR_FEATURES i, float weight);
	void TuneHistogramWeights(HISTOGRAM_FEATURES i, float weight);

	int getIndex(std::string file_name);

	//float m_distances[NUM_SHAPES][NUM_SHAPES]; // STORE THE DISTANCES AS 2D ARRAY OF FLOATS ( I THINK THIS IS TOO LARGE TO STORE IN MEMORY)
	std::vector<std::string> m_name_map;

	std::unordered_map<int, float> m_scalar_weights = {
		{SCALAR_FEATURES::DIAMETER,			1.0f},
		{SCALAR_FEATURES::BB_DIAMETER,		1.0f},
		{SCALAR_FEATURES::BB_VOLUME,		1.0f},
		{SCALAR_FEATURES::SURFACEAREA,		1.0f},
		{SCALAR_FEATURES::VOLUME,			1.0f},
		{SCALAR_FEATURES::VOLUMECOMPS,		1.0f},
		{SCALAR_FEATURES::CONVEXITY,		1.0f},
		{SCALAR_FEATURES::ECCENTRICITY02,	1.0f},
		{SCALAR_FEATURES::ECCENTRICITY01,	1.0f},
		{SCALAR_FEATURES::ECCENTRICITY12,	1.0f},
		{SCALAR_FEATURES::COMPACTNESS,		1.0f},
		{SCALAR_FEATURES::SPHERICITY,		1.0f},
		{SCALAR_FEATURES::RECTANGULARITY,	1.0f}
	};
	
	std::unordered_map<int, std::string> m_scalar_feature_names = {
		{SCALAR_FEATURES::DIAMETER,			"Diameter"},
		{SCALAR_FEATURES::BB_DIAMETER,		"BB Diameter"},
		{SCALAR_FEATURES::BB_VOLUME,		"BB Volume"},
		{SCALAR_FEATURES::SURFACEAREA,		"Surface Area"},
		{SCALAR_FEATURES::VOLUME,			"Volume"},
		{SCALAR_FEATURES::VOLUMECOMPS,		"Volume Comps"},
		{SCALAR_FEATURES::CONVEXITY,		"Convexity"},
		{SCALAR_FEATURES::ECCENTRICITY02,	"Eccentricity 02"},
		{SCALAR_FEATURES::ECCENTRICITY01,	"Eccentricity 01"},
		{SCALAR_FEATURES::ECCENTRICITY12,	"Eccentricity 12"},
		{SCALAR_FEATURES::COMPACTNESS,		"Compactness"},
		{SCALAR_FEATURES::SPHERICITY,		"Sphericity"},
		{SCALAR_FEATURES::RECTANGULARITY,	"Rectangularity"}
	};

	std::unordered_map<int, float> m_histogram_weights = {
		{HISTOGRAM_FEATURES::A1, 1.0f},
		{HISTOGRAM_FEATURES::D1, 1.0f},
		{HISTOGRAM_FEATURES::D2, 1.0f},
		{HISTOGRAM_FEATURES::D3, 1.0f},
		{HISTOGRAM_FEATURES::D4, 1.0f}
	};

	std::unordered_map<int, std::string> m_histogram_feature_names = {
		{HISTOGRAM_FEATURES::A1, "A1"},
		{HISTOGRAM_FEATURES::D1, "D1"},
		{HISTOGRAM_FEATURES::D2, "D2"},
		{HISTOGRAM_FEATURES::D3, "D3"},
		{HISTOGRAM_FEATURES::D4, "D4"}
	};


private:
	float m_features[NUM_SHAPES][NUM_FEATURES]; // STORE THE FEATURES AS 2D ARRAY OF FLOATS
	//std::unordered_map<std::string, int> m_name_map; // MAP THE SHAPE INDEX TO THE SHAPE NAME (E.G. AIRCRAFTBUOYANT/M1339.OBJ)
	std::vector<std::string> m_class_map; // MAP THE SHAPE INDEX TO THE SHAPE NAME

	std::unordered_map<HISTOGRAM_FEATURES, std::pair<int, int>> m_histogram_ranges = {
		{HISTOGRAM_FEATURES::A1, {13, 73}},
		{HISTOGRAM_FEATURES::D1, {73, 177}},
		{HISTOGRAM_FEATURES::D2, {177, 251}},
		{HISTOGRAM_FEATURES::D3, {251, 311}},
		{HISTOGRAM_FEATURES::D4, {311, 363}}
	};

	void parseFeatures();
	void standardizeFeatures();

	float computeHistogramDistance(int i, int j, HISTOGRAM_FEATURES hist_type);
};
