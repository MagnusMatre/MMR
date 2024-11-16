#include "Ann.h"

ANN::ANN(std::string& feature_file, std::string& load_tree_file, double beta, int n_trees) {
	m_feature_file = feature_file;

	m_query_engine = std::make_unique<QueryEngine>();
	m_query_engine->LoadFeatures(m_feature_file);
	m_query_engine->Initialize(STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7);

	m_beta = beta;
	m_n_trees = n_trees;

	if (load_tree_file == "") {
		BuildTree();
	}
	else {
		LoadTree(load_tree_file);
	}
}

ANN::~ANN() {
}

double ANN::BuildTree() {
	// Delete current m_t if it exists
	if (m_t != nullptr) {
		delete m_t;
	}

	// Create a new tree
	m_t = new Annoy::AnnoyIndex<int, double, Annoy::Manhattan, Annoy::Kiss64Random, Annoy::AnnoyIndexSingleThreadedBuildPolicy>(NUM_FEATURES);

	// Add the features to the tree
	for (int i = 0; i < NUM_SHAPES; ++i) {
		double* vec = (double*)malloc(NUM_FEATURES * sizeof(double));

		for (int z = 0; z < NUM_FEATURES; ++z) { // FORGOT TO SKIP FEATURES THAT WE DECIDED NOT TO USE (like squared diameter, ...) oh well
			if (z >= 19) {
				vec[z] = (m_beta * m_query_engine->m_features[i][z]);
			}
			else {
				vec[z] = ((1 - m_beta) * m_query_engine->m_features[i][z]);
			}
		}

		m_t->add_item(i, vec);
	}

	std::cout << "Builing tree..." << std::endl;
	const auto start = std::chrono::high_resolution_clock::now();
	m_t->build(m_n_trees);
	const auto end = std::chrono::high_resolution_clock::now();

	const std::chrono::duration<double> elapsed = end - start;
	std::cout << "Tree built in " << elapsed.count() << " seconds" << std::endl;

	return elapsed.count();
}

std::tuple<std::vector<int>, std::vector<double>, double> ANN::GetClosest(int K, int search_k, int q) {

	std::vector<int> closest;
	std::vector<double> distances;

	auto start = std::chrono::high_resolution_clock::now();
	m_t->get_nns_by_item(q, K, search_k, &closest, &distances);
	auto end = std::chrono::high_resolution_clock::now();

	const std::chrono::duration<double> query_time = end - start;

	return { closest, distances , query_time.count() };

}

void ANN::LoadTree(std::string& file_name) {
	// Delete current m_t if it exists
	if (m_t != nullptr) {
		delete m_t;
	}

	// Create a new tree
	m_t = new Annoy::AnnoyIndex<int, double, Annoy::Manhattan, Annoy::Kiss64Random, Annoy::AnnoyIndexSingleThreadedBuildPolicy>(NUM_FEATURES);

	m_t->load(file_name.c_str());
}