#include "QueryBenchmark.h"

QueryBenchmark::QueryBenchmark(std::string& feature_file, std::string& save_directory) {
	m_feature_file = feature_file;
	m_save_directory = save_directory;

	m_queryEngine = std::make_unique<QueryEngine>();

	getFrequencies();
	return;
}

QueryBenchmark::~QueryBenchmark() {
}

void QueryBenchmark::ComputeDistanceMatrix() {
	m_queryEngine->LoadFeatures(m_feature_file);
	m_queryEngine->Initialize(STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::ABSOLUTE, 0.7f);
	m_queryEngine->ComputeFullDistanceMatrix();
}

void QueryBenchmark::RunBenchmark(int K, STANDARDIZATION_TYPE s_type, STANDARDIZATION_TYPE s_type_histogram, DISTANCE_TYPE d_type_scalar, DISTANCE_TYPE d_type_histogram, float gamma) {
	m_queryEngine->LoadFeatures(m_feature_file);
	m_queryEngine->Initialize(s_type, s_type_histogram, d_type_scalar, d_type_histogram, gamma);

	std::string compact_save_name = m_save_directory + "/compact.txt";
	std::string save_name = m_save_directory + "/" + std::to_string(s_type) + std::to_string(s_type_histogram) + std::to_string(d_type_scalar) + std::to_string(d_type_histogram) + "_" + std::to_string(gamma) + ".txt";

	// Open results file
	std::ofstream compact_results_file;
	std::ofstream results_file;
	results_file.open(save_name);
	compact_results_file.open(compact_save_name, std::ios_base::app);

	float cur_total_score = 0;
	float cur_total_corrected_score = 0;

	for (int i = 0; i < NUM_QUERY_SHAPES; i++) {
		//std::vector<std::pair<float, std::string>> results = m_queryEngine->DoBenchmark(m_queryIndices[i], K);
		std::vector<std::pair<float, std::string>> results = m_queryEngine->DoBenchmark(i, K);
		float cur_score = 0;

		//std::string class_name = m_queryEngine->getClassName(m_queryIndices[i]);
		//std::string file_name = m_queryEngine->getObjectName(m_queryIndices[i]);
		std::string class_name = m_queryEngine->getClassName(i);
		std::string file_name = m_queryEngine->getObjectName(i);

		// If the current class name is the same as the query object, then inrcease the scores
		for (int j = 0; j < K; j++) {
			if (m_queryEngine->getClassName(m_queryEngine->getIndex(results[j].second)) == class_name) {
				cur_score += 1.0f;
			}
		}

		cur_total_score += cur_score;
		//cur_total_corrected_score += m_frequencies[i] * (cur_score / NUM_QUERY_SHAPES);

		// Write the results to the file
		//results_file << "Query Object: " << m_queryIndices[i] << ": " << m_queryEngine->getObjectName(m_queryIndices[i]) << " : s " << cur_score << std::endl;
		results_file << "Query Object: " << i << ": " << m_queryEngine->getObjectName(i) << " : s " << cur_score << std::endl;
		for (int j = 0; j < K; j++) {
			results_file << results[j].second << " " << results[j].first << std::endl;
		}
		results_file << "---------------------------------------------------------------" << std::endl;
		results.clear();

		std::cout << "Tested " << i << " query shapes: " << cur_total_score / (i+1) << std::endl;

	}

	// Write the total scores to the file
	results_file << "Total Score: " << cur_total_score / NUM_QUERY_SHAPES << std::endl;
	//results_file << "Total Corrected Score: " << cur_total_corrected_score << std::endl;

	compact_results_file << std::to_string(s_type) << std::to_string(s_type_histogram) << std::to_string(d_type_scalar) << std::to_string(d_type_histogram) << "_" << std::to_string(gamma) << " " << cur_total_score / NUM_QUERY_SHAPES << std::endl;

	results_file.close();
	compact_results_file.close();
}

void QueryBenchmark::RunBenchmarkScalarWeights(int K, STANDARDIZATION_TYPE s_type, STANDARDIZATION_TYPE s_type_histogram, DISTANCE_TYPE d_type_scalar, DISTANCE_TYPE d_type_histogram, float gamma, std::vector<float> scalar_weights) {
	m_queryEngine->LoadFeatures(m_feature_file);
	m_queryEngine->Initialize(s_type, s_type_histogram, d_type_scalar, d_type_histogram, gamma);

	//Set the weights
	for (int k = 0; k < NUM_SCALAR_FEATURES; k++) {
		m_queryEngine->TuneScalarWeights((SCALAR_FEATURES)k, scalar_weights[k]);
	}

	std::string compact_save_name = m_save_directory + "/compact.txt";
	std::string save_name = m_save_directory + "/";

	std::string scalar_weight_string = "";

	for (int k = 0; k < NUM_SCALAR_FEATURES; k++) {
		scalar_weight_string += std::to_string(scalar_weights[k]) + "_";
	}
	save_name = save_name + scalar_weight_string +  ".txt";

	// Open results file
	std::ofstream compact_results_file;
	std::ofstream results_file;
	results_file.open(save_name);
	compact_results_file.open(compact_save_name, std::ios_base::app);

	float cur_total_score = 0;
	float cur_total_corrected_score = 0;

	for (int i = 0; i < NUM_QUERY_SHAPES; i++) {
		std::vector<std::pair<float, std::string>> results = m_queryEngine->DoBenchmark(m_queryIndices[i], K);
		//std::vector<std::pair<float, std::string>> results = m_queryEngine->DoBenchmark(i, K);
		float cur_score = 0;

		std::string class_name = m_queryEngine->getClassName(m_queryIndices[i]);
		std::string file_name = m_queryEngine->getObjectName(m_queryIndices[i]);
		//std::string class_name = m_queryEngine->getClassName(i);
		//std::string file_name = m_queryEngine->getObjectName(i);

		// If the current class name is the same as the query object, then inrcease the scores
		for (int j = 0; j < K; j++) {
			if (m_queryEngine->getClassName(m_queryEngine->getIndex(results[j].second)) == class_name) {
				cur_score += 1.0f;
			}
		}

		cur_total_score += cur_score;
		//cur_total_corrected_score += m_frequencies[i] * (cur_score / NUM_QUERY_SHAPES);

		// Write the results to the file
		results_file << "Query Object: " << m_queryIndices[i] << ": " << m_queryEngine->getObjectName(m_queryIndices[i]) << " : s " << cur_score << std::endl;
		//results_file << "Query Object: " << i << ": " << m_queryEngine->getObjectName(i) << " : s " << cur_score << std::endl;
		for (int j = 0; j < K; j++) {
			results_file << results[j].second << " " << results[j].first << std::endl;
		}
		results_file << "---------------------------------------------------------------" << std::endl;
		results.clear();

		std::cout << "Tested " << i << " query shapes: " << cur_total_score / (i + 1) << std::endl;

	}

	// Write the total scores to the file
	results_file << "Total Score: " << cur_total_score / NUM_QUERY_SHAPES << std::endl;
	//results_file << "Total Corrected Score: " << cur_total_corrected_score << std::endl;

	compact_results_file << scalar_weight_string << " " << cur_total_score / NUM_QUERY_SHAPES << std::endl;

	results_file.close();
	compact_results_file.close();
}

void QueryBenchmark::RunBenchmarkHistogramWeights(int K, STANDARDIZATION_TYPE s_type, STANDARDIZATION_TYPE s_type_histogram, DISTANCE_TYPE d_type_scalar, DISTANCE_TYPE d_type_histogram, float gamma, std::vector<float> histogram_weights) {
	m_queryEngine->LoadFeatures(m_feature_file);
	m_queryEngine->Initialize(s_type, s_type_histogram, d_type_scalar, d_type_histogram, gamma);

	//Set the weights
	for (int k = 0; k < NUM_HISTOGRAM_FEATURES; k++) {
		m_queryEngine->TuneHistogramWeights((HISTOGRAM_FEATURES)k, histogram_weights[k]);
	}

	std::string compact_save_name = m_save_directory + "/compact.txt";
	std::string save_name = m_save_directory + "/";

	std::string histogram_weight_string = "";

	for (int k = 0; k < NUM_HISTOGRAM_FEATURES; k++) {
		histogram_weight_string += std::to_string(histogram_weights[k]) + "_";
	}
	save_name = save_name + histogram_weight_string + ".txt";

	// Open results file
	std::ofstream compact_results_file;
	std::ofstream results_file;
	results_file.open(save_name);
	compact_results_file.open(compact_save_name, std::ios_base::app);

	float cur_total_score = 0;
	float cur_total_corrected_score = 0;

	for (int i = 0; i < NUM_QUERY_SHAPES; i++) {
		std::vector<std::pair<float, std::string>> results = m_queryEngine->DoBenchmark(m_queryIndices[i], K);
		//std::vector<std::pair<float, std::string>> results = m_queryEngine->DoBenchmark(i, K);
		float cur_score = 0;

		std::string class_name = m_queryEngine->getClassName(m_queryIndices[i]);
		std::string file_name = m_queryEngine->getObjectName(m_queryIndices[i]);
		//std::string class_name = m_queryEngine->getClassName(i);
		//std::string file_name = m_queryEngine->getObjectName(i);

		// If the current class name is the same as the query object, then inrcease the scores
		for (int j = 0; j < K; j++) {
			if (m_queryEngine->getClassName(m_queryEngine->getIndex(results[j].second)) == class_name) {
				cur_score += 1.0f;
			}
		}

		cur_total_score += cur_score;
		//cur_total_corrected_score += m_frequencies[i] * (cur_score / NUM_QUERY_SHAPES);

		// Write the results to the file
		results_file << "Query Object: " << m_queryIndices[i] << ": " << m_queryEngine->getObjectName(m_queryIndices[i]) << " : s " << cur_score << std::endl;
		//results_file << "Query Object: " << i << ": " << m_queryEngine->getObjectName(i) << " : s " << cur_score << std::endl;
		for (int j = 0; j < K; j++) {
			results_file << results[j].second << " " << results[j].first << std::endl;
		}
		results_file << "---------------------------------------------------------------" << std::endl;
		results.clear();

		std::cout << "Tested " << i << " query shapes: " << cur_total_score / (i + 1) << std::endl;

	}

	// Write the total scores to the file
	results_file << "Total Score: " << cur_total_score / NUM_QUERY_SHAPES << std::endl;
	//results_file << "Total Corrected Score: " << cur_total_corrected_score << std::endl;

	compact_results_file << histogram_weight_string << " " << cur_total_score / NUM_QUERY_SHAPES << std::endl;

	results_file.close();
	compact_results_file.close();
}

void QueryBenchmark::WeightOptimizer(int max_iterations) {

	// In each iteration, keep pick a few features that you will change simultaneously. Then for those, randomly increase or decrease them.
	// if this yields an improvement in the score, keep it and move on. Otherwise, pick some different features and try again.

	m_queryEngine->LoadFeatures(m_feature_file);
	m_queryEngine->Initialize(STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ABSOLUTE, DISTANCE_TYPE::ABSOLUTE, 0.5f);

	// Open a log
	std::ofstream log_file;
	log_file.open(m_save_directory + "/weight_optimization_log.txt");

	std::ofstream cur_weights;

	float weights[24] = {0};
	float gamma = 0.5f;

	// Initialize the weights
	for (int k = 0; k < NUM_SCALAR_FEATURES; k++) {
		weights[k] = m_queryEngine->m_scalar_weights[k];
	}
	for (int k = 0; k < NUM_HISTOGRAM_FEATURES; k++) {
		weights[k + NUM_SCALAR_FEATURES] = m_queryEngine->m_histogram_weights[k];
	}

	/*cur_weights.open(m_save_directory + "/cur_best_weights.txt", std::ios_base::app);
	for (int k = 0; k < NUM_SCALAR_FEATURES; k++) {
		cur_weights << m_queryEngine->m_scalar_weights[k] << ", ";
	}
	for (int k = 0; k < NUM_HISTOGRAM_FEATURES; k++) {
		cur_weights << m_queryEngine->m_histogram_weights[k] << ", ";
	}
	cur_weights.close();*/

	double cur_score = computeScore();

	std::cout << "Initial score: " << cur_score << std::endl;

	for (int it = 0; it < max_iterations; it++) {
		// Save the current feature values to 

		double new_score = cur_score;
		int cnt = 0;
		double cur_factor = 0.25;
		while (new_score <= cur_score) {
			//Reset the weights to the original values
			for (int k = 0; k < NUM_SCALAR_FEATURES; k++) {
				m_queryEngine->TuneScalarWeights((SCALAR_FEATURES)k, weights[k]);
			}
			for (int k = 0; k < NUM_HISTOGRAM_FEATURES; k++) {
				m_queryEngine->TuneHistogramWeights((HISTOGRAM_FEATURES)k, weights[NUM_SCALAR_FEATURES + k]);
			}
			m_queryEngine->TuneGamma(gamma);

			//Adjust scalar weights
			for (int k = 0; k < NUM_SCALAR_FEATURES; k++) {
				if (k == 5 || k == 14 || k == 16) { continue; } // Not using these features

				// with probability 25%, increase the weight, with probability 25% decrease the weight and with probability 50%, dont change
				int u = std::rand() % 4;
				if (u == 0) {
					m_queryEngine->TuneScalarWeights((SCALAR_FEATURES)k, m_queryEngine->m_scalar_weights[k] + cur_factor);
				}
				else if (u == 1) {
					m_queryEngine->TuneScalarWeights((SCALAR_FEATURES)k, std::min(0.0, m_queryEngine->m_scalar_weights[k] - cur_factor));
				}

			}

			// Compute histogram feature improvements
			for (int k = 0; k < NUM_HISTOGRAM_FEATURES; k++) {
				// with probability 25%, increase the weight, with probability 25% decrease the weight and with probability 50%, dont change
				int u = std::rand() % 4;
				if (u == 0) {
					m_queryEngine->TuneHistogramWeights((HISTOGRAM_FEATURES)k, m_queryEngine->m_scalar_weights[k] + cur_factor);
				}
				else if (u == 1) {
					m_queryEngine->TuneHistogramWeights((HISTOGRAM_FEATURES)k, std::min(0.0, m_queryEngine->m_scalar_weights[k] - cur_factor));
				}

			}

			int u = std::rand() % 10; // Increase or decrease gamma with small percentage
			if (u == 0) {
				m_queryEngine->TuneGamma(std::max(0.7, gamma + 0.05 ));
			}
			else if (u == 1) {
				m_queryEngine->TuneGamma(std::min(0.3, gamma - 0.05));
			}
			

			// Compute the new score
			new_score = computeScore();
			cnt++;

			// Each time the counter reaches a multiple of 10, increase the factor by which we scale the weights
			if (cnt % 10 == 0) {
				cur_factor = std::min(2.0, cur_factor+0.25);
			}

		}

		cur_score = new_score;
		std::cout << "Iteration " << it << ": Found improvement after " << cnt << " attempts, new score: " << new_score << std::endl;

		log_file << "Iteration " << it << ": Found improvement after " << cnt << " attempts, new score: " << new_score << std::endl;
		log_file << "Gamma="<< m_queryEngine->getGamma() << " Weights: ";
		//Save the new weights in the weights array and save them to file
		for (int k = 0; k < NUM_SCALAR_FEATURES; k++) {
			weights[k] = m_queryEngine->m_scalar_weights[k];
			log_file << m_queryEngine->m_scalar_weights[k] << ", ";

		}
		for (int k = 0; k < NUM_HISTOGRAM_FEATURES; k++) {
			weights[k + NUM_SCALAR_FEATURES] = m_queryEngine->m_histogram_weights[k];
			log_file << m_queryEngine->m_histogram_weights[k] << ", ";
		}
		gamma = m_queryEngine->getGamma();
	
	}

	log_file.close();
	
}

void QueryBenchmark::getFrequencies() {
	int totalFrequency = 0;

	for (int i = 0; i < NUM_QUERY_SHAPES; i++) {
		m_frequencies[i] = m_queryEngine->getFrequency(m_queryIndices[i]);
		totalFrequency += m_queryEngine->getFrequency(m_queryIndices[i]);
	}

	for (int i = 0; i < NUM_QUERY_SHAPES; i++) {
		m_frequencies[i] /= totalFrequency;
	}
}

double QueryBenchmark::computeScore() {

	double cur_total_score = 0;

	for (int i = 0; i < NUM_QUERY_SHAPES; i++) {
		std::vector<std::pair<float, std::string>> results = m_queryEngine->DoBenchmark(m_queryIndices[i], 10);

		int cur_score = 0;

		std::string class_name = m_queryEngine->getClassName(m_queryIndices[i]);
		std::string file_name = m_queryEngine->getObjectName(m_queryIndices[i]);

		// If the current class name is the same as the query object, then inrcease the scores
		for (int j = 0; j < 10; j++) {
			if (m_queryEngine->getClassName(m_queryEngine->getIndex(results[j].second)) == class_name) {
				cur_score += 1;
			}
		}

		cur_total_score += (double)cur_score / NUM_QUERY_SHAPES;
	}
	return cur_total_score;
}

void QueryBenchmark::ComputeDistanceStats(DISTANCE_TYPE hist_dist_type) {
	m_queryEngine->LoadFeatures(m_feature_file);
	m_queryEngine->Initialize(STANDARDIZATION_TYPE::NO, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::EUCLIDEAN, hist_dist_type, 0);
	m_queryEngine->ComputeHistogramDistanceStatistics();
}