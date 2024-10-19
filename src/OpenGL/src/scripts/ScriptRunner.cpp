#define NOMINMAX
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN


#include <iostream>
#include <string>

// Include any necessary headers for your scripts
#include "CGALtest.h"
#include "CGALclean.h"
#include "Script1.h"
#include "Script2.h"
#include "PMPremesh.h"
#include "VCGrepair.h"
#include "VCGremesh.h"
#include "check_meshes.h"
#include "FeatureScript.h"

void RunScript1() {
    // Code to run Script1
    Script1 script;
    script.Execute();
}

void RunScript2() {
    // Code to run Script2
    Script2 script;
    script.Execute();
}

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


	std::string input_dir = "../../res/convex_hulls_okaymeshes";
	std::string output_dir = "../../res/convex_hulls_okaymeshes";
	std::string start_dir = "AircraftBuoyant";


	CGALclean script(input_dir, output_dir, 4000, 500); 
	script.handle_all_directories(start_dir);
}

void RunScriptFeatureScript() {
	std::string mesh_dir = "../../data/OkayMeshes/Bed";
	std::string input_directory = "../../data/OkayMeshes"; // unused...
	std::string output_directory = "../../res/features_okaymeshes"; // unused, features are just saved to /features.txt file inside of the mest_dir directory (for now)
	std::string start_dir = "Musical_Instrument";

	FeatureScript script(input_directory, output_directory, false);
	//script.test_mesh(mesh_dir);
	script.test_directory(mesh_dir);
	//script.handle_all_directories(start_dir);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ScriptRunner <script_name>" << std::endl;
        return 1;
    }

    std::string scriptName = argv[1];


    if (scriptName == "Script1") {
        RunScript1();
    } else if (scriptName == "Script2") {
        RunScript2();
	}
	else if (scriptName == "PMPremesh") {
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
    else {
        std::cerr << "Unknown script: " << scriptName << std::endl;
        return 1;
    }

    return 0;
}
