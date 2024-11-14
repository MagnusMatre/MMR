#pragma once

#include <annoy/annoylib.h>
#include <annoy/kissrandom.h>

#include "Ann.h"

#include <memory>
#include <map>
#include <random>
#include <iostream>
#include <fstream>
#include <chrono>

class RunAnn {
public:
	RunAnn();
	~RunAnn();

	void ExecuteDistances(std::string distance_name);
	void ExecuteBeta(std::string& feature_file, int steps);
	void Execute_n_trees(std::string& feature_file, std::vector<int>& n_trees_values);
	void Execute_k_search(std::string& feature_file, std::vector<int>& k_search_values);
	void ExecuteQuerySize(std::string& feature_file, std::vector<int> K_values);
	void ExecuteDistanceMatrix(std::string& feature_file, int K);


	double Initialize(std::string& feature_file, std::string& load_tree_file, double beta, int n_trees);

	std::pair<double, double> ComputeScores(int K, int search_k);

private:
	
	std::unique_ptr<ANN> m_ann;

	std::string m_feature_file;
};