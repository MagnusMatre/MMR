#define NOMINMAX
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN


#include <iostream>
#include <string>

// Include any necessary headers for your scripts
#include "FullRemesh.h"
#include "NormalizeMeshes.h"
#include "check_meshes.h"
#include "FeatureScript.h"
#include "QueryBenchmark.h"
#include "RunAnn.h"

void RunScriptFullRemesh() {
	std::string input_dir = "../../data/MeshesFiltered";
	std::string output_dir = "../../data/MeshesFilteredRemeshed";

	FullRemesh script(input_dir, output_dir, 4000);
	script.handle_all_directories();
}

void RunNormalizationScript() {
	/*std::string input_directory = "../../data/Primitives";
	std::string output_directory = "../../data/PrimitivesNormalized";*/

	std::string input_directory = "../../data/MeshesFilteredRemeshed";
	std::string output_directory = "../../data/MeshesFilteredNormalized";

	NormalizeMeshes script(input_directory, output_directory);
	script.handle_all_directories();
}

void RunConvexHullAndSamplePointsScript() {
	std::string input_directory = "../../data/MeshesFilteredRemeshed";
	std::string output_directory = "../../res";
	std::string start_dir = "";

	FeatureScript script(input_directory, output_directory, true, true);
	script.handle_all_directories(start_dir);
}

void RunScriptFeatureScript() {
	std::string input_directory = "../../data/MeshesFilteredNormalized";
	std::string output_directory = ""; // Unused in this case
	std::string start_dir = "";


	FeatureScript script(input_directory, output_directory, true, false);
	script.handle_all_directories(start_dir);
}



void RunScriptQueryBenchmark() {
	std::string feature_file = "../../res/features_final.txt";
	std::string save_folder = "../../res/query_results"; // Change this according to the tests you are performing


	QueryBenchmark script(feature_file, save_folder);

	script.RunBenchmarkScalarDistances();
	script.RunBenchmarkHistogramDistances();
	script.RunBenchmarkGamma();
	script.RunBenchmarkScalarWeights_();
	script.RunBenchmarkHistogramWeights_();

	// Compute min, max, mean and std of different distance functions (time consuming, already done)
	//script.ComputeDistanceStats(DISTANCE_TYPE::MYABSOLUTE);
	//script.ComputeDistanceStats(DISTANCE_TYPE::EUCLIDEAN);
	//script.ComputeDistanceStats(DISTANCE_TYPE::COSINE);
	//script.ComputeDistanceStats(DISTANCE_TYPE::EMD);

	//script.WeightOptimizer(1000); // illegal...

	// Compute the distance matrix
	//script.ComputeDistanceMatrix();

	std::vector<int> K_values = { 1, 2, 5, 10, 15, 20, 35, 50, 75, 100, 150, 200, 350 };
	script.RunBenchmarkTiming(K_values);
	
}

void RunScriptRunAnn() {
	// Code to run Ann
	std::string feature_file = "../../res/features_final.txt";
	std::string m_save_root = "../../res/scalability_results";
	std::string load_tree_file = "";


	RunAnn script(m_save_root);

	// Note, the distance function can only be set in the Ann.h file MANUALLY!
	//script.Initialize(feature_file, load_tree_file, 0.5);
	//script.ExecuteDistances("DotProduct");

	
	script.ExecuteBeta(feature_file, 20); // Conclusion -> beta = 0.55

	std::vector<int> n_trees_values = { 1, 5, 10, 20, 50, 100, 200, 500, 1000, 2000 };
	script.Execute_n_trees(feature_file, n_trees_values); // Conclusion -> n_trees = 100

	std::vector<int> k_search_values = { 1, 5, 10, 20, 50, 100, 200, 500, 1000, 2000 };
	script.Execute_k_search(feature_file, k_search_values); // Conclusion -> k_search = 20

	std::vector<int> K_values = { 1, 2, 5, 10, 15, 20, 35, 50, 75, 100, 150, 200, 350 };
	//std::vector<int> K_values = { 500, 1000, 1500, 2000 }; // This crashes since it does not retrieve the correct number of shapes
	script.ExecuteQuerySize(feature_file, K_values); 

	//script.ExecuteDistanceMatrix(feature_file, 350);
}


int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Usage: ScriptRunner <script_name>" << std::endl;
		return 1;
	}

	std::string scriptName = argv[1];


	if (scriptName == "FullRemesh") {
		RunScriptFullRemesh();
	}
	else if (scriptName == "Normalize") {
		RunNormalizationScript();
	}
	else if (scriptName == "HullsAndSamplePoints"){
		RunConvexHullAndSamplePointsScript();
	}
	else if (scriptName == "FeatureExtraction") {
		RunScriptFeatureScript();
	}
	else if (scriptName == "QueryExperiments") {
		RunScriptQueryBenchmark();
	}
	else if (scriptName == "ScalabilityExperiments") {
		RunScriptRunAnn();
	}
    else {
        std::cerr << "Unknown script: " << scriptName << std::endl;
        return 1;
    }

    return 0;
}
