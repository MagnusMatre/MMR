#pragma once

#include <iostream>

#include <Normalization.h>

class NormalizeMeshes {

public:
	NormalizeMeshes(std::string& mesh_directory, std::string& output_directory);
	~NormalizeMeshes();

	void handle_all_directories();

private:
	std::string m_mesh_directory;
	std::string m_output_dir;

};