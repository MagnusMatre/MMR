#define NOMINMAX
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN


#include <iostream>
#include <string>

// Include any necessary headers for your scripts
#include "CGALtest.h"
#include "CGALclean.h"
#include "PMPremesh.h"
#include "VCGrepair.h"
#include "VCGremesh.h"
#include "check_meshes.h"
#include "FeatureScript.h"
#include "QueryBenchmark.h"

void RunScriptPMPremesh() {
	// Code to run PMPtest
    //std::string input_dir_name = "../../data/Meshes/AircraftBuoyant";
    //std::string output_dir_name = "../../data/RefinedMeshes";
	std::string input_dir = "../../data/NormalizedMeshes_3";
	std::string output_dir = "../../data/NormalizedMeshes_3_noholes";
    std::string start_dir = "AircraftBuoyant";

	//std::string file_name = "m1339.obj";
	
	PMPremesh Remesher(input_dir, output_dir, 4000, 500); 
	Remesher.handle_all_meshes(start_dir);
	//Remesher.handle_mesh(file_name);

}

void RunScriptVCGrepair() {
	// Code to run VCGrepair
	std::string input_mesh = "../../data/VCGremeshedMeshes/Bus";
	std::string output_dir = "../../data/RepairedMeshes";

	VCGrepair script(input_mesh, output_dir);
	script.Execute();
}

void RunScriptVCGremesh() {
	// Code to run VCGremesh
	/*std::string input_dir = "../../data/CGALsimple_refine";
	std::string output_dir = "../../data/VCGdecimated";*/
	std::string input_dir = "../../data/VCGdecimatedCleaned";
	std::string output_dir = "../../data/VCGdecimated";
	std::string start_dir = "AircraftBuoyant";

	VCGremesh script(input_dir, output_dir);
    script.remesh_all_directories(start_dir);
	//script.Execute();
}

void RunScriptCheckMeshes() {
	// Code to run CheckMeshes
	std::string input_dir = "../../data/Meshes";
	//std::string output_dir = "../../data/RefinedMeshes";
	//std::string start_dir = "AircraftBuoyant";

	check_meshes script;
	script.Execute(input_dir);
	//script.Execute();
}

void RunScriptCGALtest() {
	// Code to run CGALtest
	CGALtest script;
	script.Execute();

}

void RunScriptCGALclean() {
	// Code to run CGALclean
	/*std::string input_dir = "../../data/VCGdecimated";
	std::string output_dir = "../../data/VCGdecimatedCleaned";*/


	std::string input_dir = "../../res/convex_hulls_okaymeshes3";
	std::string output_dir = "../../res/convex_hulls_okaymeshes3";
	std::string start_dir = "AircraftBuoyant";


	CGALclean script(input_dir, output_dir, 4000, 500); 
	script.handle_all_directories(start_dir);
}

void RunScriptFeatureScript() {
	std::string mesh_dir = "../../data/OkayMeshes3/Bed";
	std::string input_directory = "../../data/OkayMeshes3";
	std::string output_directory = "../../res/sample_points3"; 
	std::string start_dir = "AircraftBuoyant";

	FeatureScript script(input_directory, output_directory, true);
	//script.test_mesh(mesh_dir);
	//script.test_directory(mesh_dir);
	script.handle_all_directories(start_dir);
}

void RunScriptQueryBenchmark() {
	std::string feature_file = "../../res/features_final.txt";
	std::string save_file = "../../res/query_results_weightoptimizer"; // Change this according to the tests you are performing


	QueryBenchmark script(feature_file, save_file);

	// Compute statistics required to normalize histogram distances
	//script.ComputeDistanceStats(DISTANCE_TYPE::ABSOLUTE);
	//script.ComputeDistanceStats(DISTANCE_TYPE::EUCLIDEAN);
	//script.ComputeDistanceStats(DISTANCE_TYPE::COSINE);
	//script.ComputeDistanceStats(DISTANCE_TYPE::EMD);

	// Run the benchmark with different standardization and distance types (NOTE: SET HISTOGRAM WEIGHTS TO ZERO)
	//script.RunBenchmark(10, STANDARDIZATION_TYPE::NO, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::EUCLIDEAN, DISTANCE_TYPE::EUCLIDEAN);
	//script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::EUCLIDEAN, DISTANCE_TYPE::EUCLIDEAN);
	//script.RunBenchmark(10, STANDARDIZATION_TYPE::STANDARD, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::EUCLIDEAN, DISTANCE_TYPE::EUCLIDEAN);
	//script.RunBenchmark(10, STANDARDIZATION_TYPE::NO, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::EUCLIDEAN);
	//script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::EUCLIDEAN); // WINNER
	//script.RunBenchmark(10, STANDARDIZATION_TYPE::STANDARD, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::EUCLIDEAN);
	//script.RunBenchmark(10, STANDARDIZATION_TYPE::NO, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ONEPOINTFIVE, DISTANCE_TYPE::EUCLIDEAN);
	//script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ONEPOINTFIVE, DISTANCE_TYPE::EUCLIDEAN); 
	//script.RunBenchmark(10, STANDARDIZATION_TYPE::STANDARD, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ONEPOINTFIVE, DISTANCE_TYPE::EUCLIDEAN);
	//script.RunBenchmark(10, STANDARDIZATION_TYPE::NO, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::COSINE, DISTANCE_TYPE::EUCLIDEAN);
	//script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::COSINE, DISTANCE_TYPE::EUCLIDEAN);
	//script.RunBenchmark(10, STANDARDIZATION_TYPE::STANDARD, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::COSINE, DISTANCE_TYPE::EUCLIDEAN);


	// Run the benchmark tests with different standardization and histogram distance function
	/*script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::EUCLIDEAN);
	script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::ABSOLUTE);
	script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::COSINE);
	script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::EMD);
	script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::RANGE, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::EUCLIDEAN);
	script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::RANGE, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::ABSOLUTE);
	script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::RANGE, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::COSINE);*/
	//script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::RANGE, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::EMD);
	/*script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::STANDARD, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::EUCLIDEAN);
	script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::STANDARD, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::ABSOLUTE);
	script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::STANDARD, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::COSINE);*/
	//script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::STANDARD, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::EMD);

	// Run the benchmark tests for optimizing gamma
	/*for (int i = 0; i < 11; i++) {
		script.RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::ABSOLUTE, float(i)/10);
	}*/

	script.WeightOptimizer(1000);
	
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ScriptRunner <script_name>" << std::endl;
        return 1;
    }

    std::string scriptName = argv[1];



	if (scriptName == "PMPremesh") {
        RunScriptPMPremesh();
    }
    else if (scriptName == "VCGrepair") {
        RunScriptVCGrepair();
    }
	else if (scriptName == "VCGremesh") {
		RunScriptVCGremesh();
	}
	else if (scriptName == "CheckMeshes") {
		RunScriptCheckMeshes();
	}
	else if (scriptName == "CGALtest") {
		RunScriptCGALtest();
	}
	else if (scriptName == "CGALclean") {
		RunScriptCGALclean();
	}
	else if (scriptName == "FeatureScript") {
		RunScriptFeatureScript();
	}
	else if (scriptName == "QueryBenchmark") {
		RunScriptQueryBenchmark();
	}
    else {
        std::cerr << "Unknown script: " << scriptName << std::endl;
        return 1;
    }

    return 0;
}
