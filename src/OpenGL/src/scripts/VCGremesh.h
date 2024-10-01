#pragma once

#include <iostream>

#include "VCGclasses.h" 

class VCGremesh {
public:
	VCGremesh(std::string& input_dir, std::string& output_dir);
	~VCGremesh();

	void Execute();
	void remesh_all_directories(std::string& start_dir);

private:
	std::string m_input_dir;
	std::string m_output_dir;

	//MyMesh m_output_mesh;
	float m_target = 2500;
	float m_threshold = 500;

	std::map<std::string, bool> m_to_skip = {
		{"AircraftBuoyant/m1339.obj", true}
	};
	

	void remesh_mesh(MyMesh& mesh, MyMesh& output_mesh);
	void edge_collapse(MyMesh& mesh, MyMesh& output_mesh);
	void decimate_mesh(MyMesh& mesh, MyMesh& output_mesh);

	void resample_mesh(MyMesh& mesh, MyMesh& output_mesh);

	void remesh_directory();
};