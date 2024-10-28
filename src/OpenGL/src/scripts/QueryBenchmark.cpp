#include "QueryBenchmark.h"

QueryBenchmark::QueryBenchmark(std::string& feature_file, std::string& save_directory) {
	m_feature_file = feature_file;
	m_save_directory = save_directory;

	m_queryEngine = std::make_unique<QueryEngine>();
	return;
}

QueryBenchmark::~QueryBenchmark() {
}

void QueryBenchmark::RunBenchmark(int K, STANDARDIZATION_TYPE s_type, STANDARDIZATION_TYPE s_type_histogram, DISTANCE_TYPE d_type_scalar, DISTANCE_TYPE d_type_histogram) {
	m_queryEngine->LoadFeatures(m_feature_file);
	m_queryEngine->Initialize(s_type, s_type_histogram, d_type_scalar, d_type_histogram);

	std::string save_name = m_save_directory + "/" + std::to_string(s_type) + std::to_string(s_type_histogram) + std::to_string(d_type_scalar) + std::to_string(d_type_histogram) + ".txt";

	// Open results file
	std::ofstream results_file;
	results_file.open(save_name);

	int totalFrequency = 0;
	
	for (int i = 0; i < 16; i++) {
		totalFrequency += m_queryEngine->getFrequency(m_queryIndices[i]);
	}

	float cur_total_score = 0;
	float cur_total_corrected_score = 0;

	for (int i = 0; i < 16; i++) {
		std::vector<std::pair<float, std::string>> results = m_queryEngine->DoBenchmark(m_queryIndices[i], K);

		float cur_score = 0;

		std::string class_name = m_queryEngine->getClassName(m_queryIndices[i]);
		std::string file_name = m_queryEngine->getObjectName(m_queryIndices[i]);

		// If the current class name is the same as the query object, then inrcease the scores
		for (int j = 0; j < K; j++) {
			if (m_queryEngine->getClassName(m_queryEngine->getIndex(results[j].second)) == class_name) {
				cur_score += 1.0f;
			}
		}

		cur_total_score += cur_score / 16;
		cur_total_corrected_score += (float)m_queryEngine->getFrequency(m_queryIndices[i]) / (float)totalFrequency * (cur_score / 16);

		// Write the results to the file
		results_file << "Query Object: " << m_queryIndices[i] << ": " << m_queryEngine->getObjectName(m_queryIndices[i]) << " : s " << cur_score << std::endl;
		for (int j = 0; j < K; j++) {
			results_file << results[j].second << " " << results[j].first << std::endl;
		}
		results_file << "---------------------------------------------------------------" << std::endl;
		results.clear();
	}

	// Write the total scores to the file
	results_file << "Total Score: " << cur_total_score << std::endl;
	results_file << "Total Corrected Score: " << cur_total_corrected_score << std::endl;

	results_file.close();
}

void QueryBenchmark::ComputeDistanceStats(DISTANCE_TYPE hist_dist_type) {
	m_queryEngine->LoadFeatures(m_feature_file);
	m_queryEngine->Initialize(STANDARDIZATION_TYPE::NO, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::EUCLIDEAN, hist_dist_type);
	m_queryEngine->ComputeHistogramDistanceStatistics();
}