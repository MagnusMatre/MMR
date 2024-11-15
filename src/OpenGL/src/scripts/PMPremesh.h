#pragma once

//include pmp libraries
#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>

// #include <pmp/algorithms/subdivision.h> // CAUSED DLL CRASH

#include <pmp/algorithms/utilities.h>
#include <pmp/algorithms/remeshing.h>
#include <pmp/algorithms/decimation.h>
#include <pmp/algorithms/hole_filling.h>

#include <filesystem>

class PMPremesh {
public:
	PMPremesh(std::string& dir_name, std::string& remesh_dir_name, unsigned int target, unsigned int threshold);
	~PMPremesh();

	void Execute();

	void handle_mesh(std::string& file_name);
	void handle_all_meshes(std::string& start_dir);

	std::string m_dir_name;
	std::string m_remesh_dir_name;

	unsigned int m_target;
	unsigned int m_threshold;

	std::unordered_map<std::string, bool> m_skip_meshes = {
		//{"City/D01010.obj", true},
		//{"City/D01066.obj", true},
		//{"Computer/m1767.obj", true}
	};

private:

	void handle_directory();

	void remesh_mesh(pmp::SurfaceMesh& mesh);

	void decimate_mesh(pmp::SurfaceMesh& mesh);

	void fill_holes(pmp::SurfaceMesh& mesh);
};