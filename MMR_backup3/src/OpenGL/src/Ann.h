#pragma once

#include <annoy/annoylib.h>
#include <annoy/kissrandom.h>

#include "QueryEngine.h"

#include <iostream>
#include <chrono>

class ANN {
public:
	ANN(std::string& feature_file, std::string& load_tree_file, double beta, int n_trees);
	~ANN();

	double BuildTree();
	void LoadTree(std::string& file_name);

	std::tuple<std::vector<int>, std::vector<double>, double> GetClosest(int K, int search_k, int q);

	Annoy::AnnoyIndex<int, double, Annoy::Manhattan, Annoy::Kiss64Random, Annoy::AnnoyIndexSingleThreadedBuildPolicy>* m_t;
	std::unique_ptr<QueryEngine> m_query_engine;

private:

	std::string m_feature_file;
	double m_beta = 0.55;
	int m_n_trees = 100;

};