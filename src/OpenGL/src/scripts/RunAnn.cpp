#include "RunAnn.h"


RunAnn::RunAnn() {

}

RunAnn::~RunAnn() {
}

double RunAnn::Initialize(std::string& feature_file, std::string& load_tree_file, double beta, int n_trees) {
	m_ann = std::make_unique<ANN>(feature_file, load_tree_file, beta, n_trees);

	double build_time = m_ann->BuildTree();
	return build_time;
}

void RunAnn::ExecuteDistances(std::string distance_name) {
	// Open a file in append mode to write the results
	std::ofstream file;
	file.open("../../res/scalability_distance_results/scores.txt", std::ios_base::app);

	// Compute the scores
	std::pair<double, double> score_time = ComputeScores(10, 10);

	// Write the results to the file
	file << distance_name << " " << score_time.first << std::endl;

	file.close();
}

void RunAnn::ExecuteBeta(std::string& feature_file, int steps) {
	std::string load_tree_file = "";

	std::ofstream file;
	file.open("../../res/scalability_beta_results/scores.txt", std::ios_base::app);

	for (int i = 0; i <= steps; ++i) {
		double beta = (double)i / steps;

		std::cout << "Beta is " << beta << std::endl;

		Initialize(feature_file, load_tree_file, beta, 2 * NUM_FEATURES);

		// Compute the scores
		std::pair<double, double> score_time = ComputeScores(10, 10);

		// Write the results to the file
		file << beta << " " << score_time.first << std::endl;

		
	}

	file.close();
}

void RunAnn::Execute_n_trees(std::string& feature_file, std::vector<int>& n_trees_values) {
	std::string load_tree_file = "";

	std::ofstream file;
	file.open("../../res/scalability_n_trees_results/scores.txt", std::ios_base::app);

	file << "NumberOfTrees,AverageScore,BuildTimeIndex,QueryTime" << std::endl;

	for (int n_tree : n_trees_values) {
		std::cout << "Number of trees is " << n_tree << std::endl;

		double build_time = Initialize(feature_file, load_tree_file, 0.55, n_tree);

		// Compute the scores
		auto start = std::chrono::high_resolution_clock::now();
		std::pair<double, double> score_time = ComputeScores(10, 10);
		auto end = std::chrono::high_resolution_clock::now();

		const std::chrono::duration<double> query_time = end - start;
		std::cout << "Query time is " << query_time.count() << std::endl;

		// Write the results to the file
		file << n_tree << "," << score_time.first << "," << build_time << "," << score_time.second << std::endl;

	}

	file.close();
}

void RunAnn::Execute_k_search(std::string& feature_file, std::vector<int>& h_search_values) {
	std::string load_tree_file = "";

	std::ofstream file;
	file.open("../../res/scalability_k_search_results/scores.txt", std::ios_base::app);

	double build_time = Initialize(feature_file, load_tree_file, 0.55, 100);

	file << "NumberOfTrees,AverageScore,QueryTime" << std::endl;

	for (int k_search : h_search_values) {
		std::cout << "k_search " << k_search << std::endl;


		// Compute the scores
		auto start = std::chrono::high_resolution_clock::now();
		std::pair<double, double> score_time = ComputeScores(10, k_search);
		auto end = std::chrono::high_resolution_clock::now();

		const std::chrono::duration<double> query_time = end - start;
		std::cout << "Query time is " << query_time.count() << std::endl;

		// Write the results to the file
		file << k_search << "," << score_time.first << "," << score_time.second << std::endl;

	}

	file.close();
}

void RunAnn::ExecuteQuerySize(std::string& feature_file, std::vector<int> K_values) {
	std::string load_tree_file = "";

	std::ofstream file;
	file.open("../../res/scalability_querysize_results/scores.txt", std::ios_base::app);

	double build_time = Initialize(feature_file, load_tree_file, 0.55, 100);

	file << "NumberOfTrees,AverageScore,QueryTime" << std::endl;

	for (int K : K_values) {

		// Compute the scores
		std::pair<double, double> score_time = ComputeScores(K, 20);

		std::cout << "Query time is " << score_time.second << std::endl;

		// Write the results to the file
		file << K << "," << score_time.first << "," << score_time.second << std::endl;

	}

	file.close();
}

std::pair<double, double> RunAnn::ComputeScores(int K, int search_k) {
	// Compute the scores
	int acc_score = 0;
	double total_time = 0;

	for (int i = 0; i < NUM_SHAPES; ++i) {

		std::cout << "Finding neighbors for " << m_ann->m_query_engine->getObjectName(i) << std::endl;

		std::tuple<std::vector<int>, std::vector<double>, double> closest_distances_time = m_ann->GetClosest(K + 1, search_k, i);
		total_time += std::get<2>(closest_distances_time);
		int score = 0;
		for (int k = 1; k < K + 1; ++k) {
			if (m_ann->m_query_engine->getClassName(std::get<0>(closest_distances_time)[k]) == m_ann->m_query_engine->getClassName(i)) {
				score += 1;
			}
		}

		acc_score += score;

		std::cout << "Current average score is " << (double)acc_score / ( K * (i + 1)) << std::endl;


	}

	return { (double)acc_score / ( K * NUM_SHAPES), total_time };

}