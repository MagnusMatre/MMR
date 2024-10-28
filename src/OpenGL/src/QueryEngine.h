#pragma once

/*
THIS CLASS IS USED FOR QUERYING THE DATABBASE.
SO IT CAN COMPUTE DISTANCES BETWEEN THE FEATURE VECTORS OF THE MESHES.
*/

#define NUM_FEATURES 519 // LENGTH OF THE FEATURE VECTOR (EXCLUDING THE CLASS NAME AND THE OBJECT NAME)
#define NUM_SHAPES 2283 // NUMBER OF SHAPES IN THE DATABASE
#define NUM_SCALAR_FEATURES 19 // NUMBER OF SCALAR FEATURES
#define NUM_HISTOGRAM_FEATURES 5 // NUMBER OF HISTOGRAM FEATURES

#define A3_START 19 
#define D1_START 119 
#define D2_START 219 
#define D3_START 319 
#define D4_START 419 

#define A3_LEN 100
#define D1_LEN 100 
#define D2_LEN 100 
#define D3_LEN 100 
#define D4_LEN 100 


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
	COMPACTNESS, // Log
	SPHERICITY, // Log
	CUBEROOTVOLUME,
	SQUAREROOTAREA,
	DIAMETERTOCUBEROOTVOLUME,
	DIAMETERTOSQUAREROOTAREA,
	DIAMETERSQUARED,
	BBCUBEROOTVOLUME,
	RECTANGULARITY
};

enum HISTOGRAM_FEATURES {
	A1,
	D1,
	D2,
	D3,
	D4
};


enum STANDARDIZATION_TYPE {
	NO,
	RANGE,
	STANDARD
};

enum DISTANCE_TYPE {
	ABSOLUTE,
	ONEPOINTFIVE,
	EUCLIDEAN,
	COSINE,
	EMD
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
	double ComputeDistance(std::string& query_obj, std::string& other_obj);
	double ComputeDistance(int query_obj, int other_obj);

	void ComputeHistogramDistanceStatistics();

	void Initialize(STANDARDIZATION_TYPE s_type, STANDARDIZATION_TYPE s_type_hist, DISTANCE_TYPE d_type_scalar, DISTANCE_TYPE d_type_histogram, float gamma);

	std::vector<std::pair<float, std::string>> DoBenchmark(int query_obj, int K); // computes the K most similar shapes to the query_obj
	
	void ComputeFullDistanceMatrix();
	void SaveDistanceMatrix(std::string& save_path);
	void LoadDistanceMatrix(std::string& load_path);

	void TuneScalarWeights(SCALAR_FEATURES i, float weight);
	void TuneHistogramWeights(HISTOGRAM_FEATURES i, float weight);
	void TuneGamma(float gamma);

	int getIndex(std::string file_name);
	std::string getClassName(int index);
	std::string getObjectName(int index);
	int getFrequency(int index);

	float getGamma();

	//float m_distances[NUM_SHAPES][NUM_SHAPES]; // STORE THE DISTANCES AS 2D ARRAY OF FLOATS ( I THINK THIS IS TOO LARGE TO STORE IN MEMORY

	std::unordered_map<int, float> m_scalar_weights = {
		{SCALAR_FEATURES::DIAMETER,					1.0f},
		{SCALAR_FEATURES::BB_DIAMETER,				1.0f},
		{SCALAR_FEATURES::BB_VOLUME,				1.0f},
		{SCALAR_FEATURES::SURFACEAREA,				1.0f},
		{SCALAR_FEATURES::VOLUME,					1.0f},
		{SCALAR_FEATURES::VOLUMECOMPS,				0.0f}, // Don't use this now
		{SCALAR_FEATURES::CONVEXITY,				1.0f},
		{SCALAR_FEATURES::ECCENTRICITY02,			1.0f},
		{SCALAR_FEATURES::ECCENTRICITY01,			1.0f},
		{SCALAR_FEATURES::ECCENTRICITY12,			1.0f},
		{SCALAR_FEATURES::COMPACTNESS,				1.0f},
		{SCALAR_FEATURES::SPHERICITY,				1.0f},
		{SCALAR_FEATURES::CUBEROOTVOLUME,			1.0f},
		{SCALAR_FEATURES::SQUAREROOTAREA,			1.0f},
		{SCALAR_FEATURES::DIAMETERTOCUBEROOTVOLUME,	0.0f}, // Don't use this
		{SCALAR_FEATURES::DIAMETERTOSQUAREROOTAREA,	1.0f},
		{SCALAR_FEATURES::DIAMETERSQUARED,			0.0f}, // Don't use this
		{SCALAR_FEATURES::BBCUBEROOTVOLUME,			1.0f},
		{SCALAR_FEATURES::RECTANGULARITY,			1.0f}
	};
	
	std::unordered_map<int, std::string> m_scalar_feature_names = {
		{SCALAR_FEATURES::DIAMETER,					"Diameter"},
		{SCALAR_FEATURES::BB_DIAMETER,				"BB Diameter"},
		{SCALAR_FEATURES::BB_VOLUME,				"BB Volume"},
		{SCALAR_FEATURES::SURFACEAREA,				"Surface Area"},
		{SCALAR_FEATURES::VOLUME,					"Volume"},
		{SCALAR_FEATURES::VOLUMECOMPS,				"Volume Comps"},
		{SCALAR_FEATURES::CONVEXITY,				"Convexity"},
		{SCALAR_FEATURES::ECCENTRICITY02,			"Eccentricity 02"},
		{SCALAR_FEATURES::ECCENTRICITY01,			"Eccentricity 01"},
		{SCALAR_FEATURES::ECCENTRICITY12,			"Eccentricity 12"},
		{SCALAR_FEATURES::COMPACTNESS,				"Compactness"},
		{SCALAR_FEATURES::SPHERICITY,				"Sphericity"},
		{SCALAR_FEATURES::CUBEROOTVOLUME,			"Cubed Root Volume"},
		{SCALAR_FEATURES::SQUAREROOTAREA,			"Square Root Area"},
		{SCALAR_FEATURES::DIAMETERTOCUBEROOTVOLUME,	"Diameter to Cubed Root Volume"},
		{SCALAR_FEATURES::DIAMETERTOSQUAREROOTAREA,	"Diameter to Square Root Area"},
		{SCALAR_FEATURES::DIAMETERSQUARED,			"Diameter Squared"},
		{SCALAR_FEATURES::BBCUBEROOTVOLUME,			"BB Cubed Root Volume"},
		{SCALAR_FEATURES::RECTANGULARITY,			"Rectangularity"}
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

	float m_features[NUM_SHAPES][NUM_FEATURES]; // STORE THE FEATURES AS 2D ARRAY OF FLOATS


private:
	//std::unordered_map<std::string, int> m_name_map; // MAP THE SHAPE INDEX TO THE SHAPE NAME (E.G. AIRCRAFTBUOYANT/M1339.OBJ)
	std::vector<std::string> m_class_map = {}; // MAP THE SHAPE INDEX TO THE SHAPE CLASS
	std::vector<std::string> m_name_map = {};  // MAP THE SHAPE INDEX TO THE SHAPE NAME

	float m_gamma = 0.5f; // Weight of all scalar distances. gamma\in[0,1].

	DISTANCE_TYPE m_distance_type_scalar;
	DISTANCE_TYPE m_distance_type_histogram;

	STANDARDIZATION_TYPE m_histogram_distance_standardization;

	int m_frequencies[69] = { 11, 19, 29, 27, 26, 44, 44, 15, 30, 37, 14,
							  81, 19, 42, 21, 17, 27, 19, 30, 30, 19, 37,
							  18, 20, 28, 20, 18, 57, 37, 15, 50, 33, 48,
							  135, 65, 120, 23, 20, 20, 20, 26, 26, 43, 21,
							  59, 16, 59, 27, 56, 61, 17, 33, 39, 60, 15,
							  11, 18, 28, 20, 52, 44, 23, 30, 23, 16, 20,
							  18, 17, 20 }; // store the number of shapes per class

	std::unordered_map<HISTOGRAM_FEATURES, std::pair<int, int>> m_histogram_ranges = {
		{HISTOGRAM_FEATURES::A1, {A3_START, A3_START + A3_LEN}},
		{HISTOGRAM_FEATURES::D1, {D1_START, D1_START + D1_LEN}},
		{HISTOGRAM_FEATURES::D2, {D2_START, D2_START + D2_LEN}},
		{HISTOGRAM_FEATURES::D3, {D3_START, D3_START + D3_LEN}},
		{HISTOGRAM_FEATURES::D4, {D4_START, D4_START + D4_LEN}}
	};

	double hist_euclidean_min_max_mu_std[5][4] = { // Stores the min, max average and standard deivation of euclidean distances for features_final.txt
		{0.00159033, 0.316422, 0.0508091, 0.00512234},
		{0.00308516, 0.664294, 0.153664, 0.00652756},
		{0.0024246, 0.325589, 0.075518, 0.0048404},
		{0.00151348, 0.379584, 0.122247, 0.00676828},
		{0.000517109, 0.591847, 0.173969, 0.00829209}
	};

	double hist_cosine_min_max_mu_std[5][4] = { // Stores the min, max average and standard deivation of cosine distances for features_final.txt
		{4.42293e-05, 0.908133, 0.105763, 0.00933547},
		{6.91977e-05, 0.999506, 0.26695, 0.0108622},
		{9.88159e-05, 0.876027, 0.127158, 0.00854967},
		{1.83753e-05, 0.98192, 0.229946, 0.0117441},
		{8.14638e-07, 0.9946, 0.287745, 0.0126892}
	};

	double hist_absolute_min_max_mu_std[5][4] = { // Stores the min, max average and standard deivation of absolute distances for features_final.txt
		{0.01176, 1.62198, 0.35835, 0.0127493},
		{0.01258, 1.99862, 0.712815, 0.0140199},
		{0.01264, 1.66654, 0.4587, 0.0121265},
		{0.005884, 1.95369, 0.693672, 0.0160605},
		{0.00143402, 1.98573, 0.814319, 0.017334}
	};

	double hist_emd_min_max_mu_std[5][4] = { // Stores the min, max average and standard deivation of EMD for features_final.txt
		{0.02242, 25.4366, 4.73597, 0.0484536},
		{0.01451, 27.2882, 4.8187, 0.0442757},
		{0.02409, 31.7407, 5.55295, 0.0487042},
		{0.00646797, 35.1219, 7.20831, 0.0572482},
		{0.00110802, 29.7176, 6.12986, 0.0533872}
	};

	void parseFeatures();
	void standardizeFeatures(STANDARDIZATION_TYPE s_type);
	double standardizeHistogramDistance(double true_ditance, HISTOGRAM_FEATURES k);

	double computeHistogramDistance(int i, int j, HISTOGRAM_FEATURES hist_type);
	double computeScalarDistance(int i, int j);
};
