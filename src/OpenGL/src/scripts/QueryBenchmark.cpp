#include "QueryBenchmark.h"

QueryBenchmark::QueryBenchmark(std::string& feature_file, std::string& save_directory) {
	m_feature_file = feature_file;
	m_save_root = save_directory;

	m_queryEngine = std::make_unique<QueryEngine>();

	getFrequencies();
	return;
}

QueryBenchmark::~QueryBenchmark() {
}

void QueryBenchmark::ComputeDistanceMatrix() {
	m_queryEngine->LoadFeatures(m_feature_file);
	m_queryEngine->Initialize(STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7f);
	m_queryEngine->ComputeFullDistanceMatrix();
}

void QueryBenchmark::RunBenchmarkScalarDistances() {
	m_save_directory = m_save_root + "/query_results_scalarfeatures";
	RunBenchmark(10, STANDARDIZATION_TYPE::NO, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::EUCLIDEAN, DISTANCE_TYPE::EUCLIDEAN, 1);
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::EUCLIDEAN, DISTANCE_TYPE::EUCLIDEAN, 1);
	RunBenchmark(10, STANDARDIZATION_TYPE::STANDARD, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::EUCLIDEAN, DISTANCE_TYPE::EUCLIDEAN, 1);
	RunBenchmark(10, STANDARDIZATION_TYPE::NO, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::EUCLIDEAN, 1);
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::EUCLIDEAN, 1); // WINNER
	RunBenchmark(10, STANDARDIZATION_TYPE::STANDARD, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::EUCLIDEAN, 1);
	RunBenchmark(10, STANDARDIZATION_TYPE::NO, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ONEPOINTFIVE, DISTANCE_TYPE::EUCLIDEAN, 1);
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ONEPOINTFIVE, DISTANCE_TYPE::EUCLIDEAN, 1); 
	RunBenchmark(10, STANDARDIZATION_TYPE::STANDARD, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::ONEPOINTFIVE, DISTANCE_TYPE::EUCLIDEAN, 1);
	RunBenchmark(10, STANDARDIZATION_TYPE::NO, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::COSINE, DISTANCE_TYPE::EUCLIDEAN, 1);
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::COSINE, DISTANCE_TYPE::EUCLIDEAN, 1);
	RunBenchmark(10, STANDARDIZATION_TYPE::STANDARD, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::COSINE, DISTANCE_TYPE::EUCLIDEAN, 1);
}

void QueryBenchmark::RunBenchmarkHistogramDistances() {
	m_save_directory = m_save_root + "/query_results_histogramfeatures";
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0); // WINNER
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::RANGE, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0);
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::STANDARD, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0);
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::EUCLIDEAN, 0);
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::RANGE, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::EUCLIDEAN, 0);
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::STANDARD, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0);
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::COSINE, 0);
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::RANGE, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::COSINE, 0);
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::STANDARD, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0);
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::EMD, 0);
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::RANGE, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::EMD, 0);
	RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::STANDARD, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0);
}

void QueryBenchmark::RunBenchmarkGamma() {
	m_save_directory = m_save_root + "/query_results_gamma";
	for (int i = 0; i < 21; i++) {
		RunBenchmark(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, float(i)/20);
	}
}

void QueryBenchmark::RunBenchmarkScalarWeights_() {
	m_save_directory = m_save_root + "/query_results_scalarweights";
	std::vector<float> scalar_weights = {
		1.0f // DIAMETER
		,1.0f // BB_DIAMETER
		,1.0f // BB_VOLUME 
		,1.0f // SURFACEAREA
		,1.0f // VOLUME
		,0.0f // VOLUMECOMPS (always disabled)
		,1.0f // CONVEXITY
		,1.0f // ECCENTRICITY02
		,1.0f // ECCENTRICITY01
		,1.0f // ECCENTRICITY12
		,1.0f // COMPACTNESS
		,1.0f // SPHERICITY
		,1.0f // CUBEROOTVOLUME
		,1.0f // SQUAREROOTAREA
		,0.0f // DIAMETERTOCUBEROOTVOLUME (always disabled)
		,1.0f // DIAMETERTOSQUAREROOTAREA
		,0.0f // DIAMETERSQUARED (always disabled)
		,1.0f // BBCUBEROOTVOLUME
		,1.0f // RECTANGULARITY
	};

	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);

	scalar_weights[0] -= 1.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[0] += 1.0f;

	scalar_weights[3] -= 1.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[3] += 1.0f;

	scalar_weights[4] -= 1.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[4] += 1.0f;

	scalar_weights[6] -= 1.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[6] += 1.0f;

	scalar_weights[7] -= 1.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[7] += 1.0f;

	scalar_weights[8] -= 1.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[8] += 1.0f;

	scalar_weights[9] -= 1.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[9] += 1.0f;

	scalar_weights[10] -= 1.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[10] += 1.0f;

	scalar_weights[11] -= 1.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[11] += 1.0f;

	scalar_weights[13] -= 1.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[13] += 1.0f;

	scalar_weights[18] -= 1.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[18] += 1.0f;

	scalar_weights = {
		1.0f // DIAMETER
		,1.0f // BB_DIAMETER
		,0.0f // BB_VOLUME (disabled after test)
		,1.0f // SURFACEAREA
		,1.0f // VOLUME
		,0.0f // VOLUMECOMPS (always disabled)
		,1.0f // CONVEXITY
		,1.0f // ECCENTRICITY02
		,1.0f // ECCENTRICITY01
		,1.0f // ECCENTRICITY12
		,1.0f // COMPACTNESS
		,1.0f // SPHERICITY
		,0.0f // CUBEROOTVOLUME (disabled after test)
		,1.0f // SQUAREROOTAREA
		,0.0f // DIAMETERTOCUBEROOTVOLUME (always disabled)
		,0.0f // DIAMETERTOSQUAREROOTAREA (disabled after test)
		,0.0f // DIAMETERSQUARED (always disabled)
		,1.0f // BBCUBEROOTVOLUME
		,1.0f // RECTANGULARITY
	};

	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);

	scalar_weights[0] *= 2.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[0] /= 2.0f;

	scalar_weights[3] *= 2.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[3] /= 2.0f;

	scalar_weights[4] *= 2.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[4] /= 2.0f;

	scalar_weights[6] *= 2.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[6] /= 2.0f;

	scalar_weights[7] *= 2.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[7] /= 2.0f;

	scalar_weights[8] *= 2.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[8] /= 2.0f;

	scalar_weights[9] *= 2.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[9] /= 2.0f;

	scalar_weights[10] *= 2.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[10] /= 2.0f;

	scalar_weights[11] *= 2.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[11] /= 2.0f;

	scalar_weights[13] *= 2.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[13] /= 2.0f;

	scalar_weights[18] *= 2.0f;
	RunBenchmarkScalarWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, scalar_weights);
	scalar_weights[18] /= 2.0f;
}

void QueryBenchmark::RunBenchmarkHistogramWeights_() {
	m_save_directory = m_save_root + "/query_results_histogramfeatures";
	std::vector<float> histogram_weights = { 1.0f,1.0f,1.0f,1.0f,1.0f };
	RunBenchmarkHistogramWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, histogram_weights);

	histogram_weights = { 0.0f,1.0f,1.0f,1.0f,1.0f };
	RunBenchmarkHistogramWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, histogram_weights);

	histogram_weights = { 1.0f,0.0f,1.0f,1.0f,1.0f };
	RunBenchmarkHistogramWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, histogram_weights);

	histogram_weights = { 1.0f,1.0f,0.0f,1.0f,1.0f };
	RunBenchmarkHistogramWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, histogram_weights);

	histogram_weights = { 1.0f,1.0f,1.0f,0.0f,1.0f };
	RunBenchmarkHistogramWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, histogram_weights);

	histogram_weights = { 1.0f,1.0f,1.0f,1.0f,0.0f };
	RunBenchmarkHistogramWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, histogram_weights);

	histogram_weights = { 2.0f,1.0f,1.0f,1.0f,1.0f };
	RunBenchmarkHistogramWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, histogram_weights);

	histogram_weights = { 1.0f,2.0f,1.0f,1.0f,1.0f };
	RunBenchmarkHistogramWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, histogram_weights);

	histogram_weights = { 1.0f,1.0f,2.0f,1.0f,1.0f };
	RunBenchmarkHistogramWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, histogram_weights);

	histogram_weights = { 1.0f,1.0f,1.0f,2.0f,1.0f };
	RunBenchmarkHistogramWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, histogram_weights);

	histogram_weights = { 1.0f,1.0f,1.0f,1.0f,2.0f };
	RunBenchmarkHistogramWeights(10, STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7, histogram_weights);
}

void QueryBenchmark::RunBenchmarkTiming(std::vector<int> K_values) {
	m_queryEngine->LoadFeatures(m_feature_file);
	m_queryEngine->Initialize(STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7f);

	std::string save_name = m_save_directory + "/scalability_normal_query/querysize_timing.txt";

	// Open results file
	std::ofstream results_file;
	results_file.open(save_name);

	results_file << "K,Score,Time" << std::endl;

	for (int i = 0; i < K_values.size(); i++) {

		double total_time = 0.0;
		double total_score = 0.0;

		for (int ii = 0; ii < NUM_SHAPES; ii++) {

			double cur_time = 0.0;
			double cur_score = 0.0;
			std::vector<std::pair<float, std::string>> results = m_queryEngine->DoBenchmarkTiming(ii, K_values[i], cur_time);

			std::string class_name = m_queryEngine->getClassName(ii);
			std::string file_name = m_queryEngine->getObjectName(ii);

			for (int j = 0; j < K_values[i]; j++) {
				if (m_queryEngine->getClassName(m_queryEngine->getIndex(results[j].second)) == class_name) {
					cur_score += 1.0f;
				}
			}
			
			total_score += cur_score;
			total_time += cur_time;

		}

		total_score /= (NUM_SHAPES * K_values[i]);

		results_file << K_values[i] << "," << total_score << "," << total_time << std::endl;
		std::cout << "Tested " << K_values[i] << " query time: " << total_time << " - score: " << total_score << std::endl;
	}
	
	results_file.close();
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
		std::vector<std::pair<float, std::string>> results = m_queryEngine->DoBenchmark(m_queryIndices[i], K);
		//std::vector<std::pair<float, std::string>> results = m_queryEngine->DoBenchmark(i, K);
		float cur_score = 0;

		std::string class_name = m_queryEngine->getClassName(m_queryIndices[i]);
		std::string file_name = m_queryEngine->getObjectName(m_queryIndices[i]);
		/*std::string class_name = m_queryEngine->getClassName(i);
		std::string file_name = m_queryEngine->getObjectName(i);*/

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
	m_queryEngine->Initialize(STANDARDIZATION_TYPE::RANGE, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::MYABSOLUTE, DISTANCE_TYPE::MYABSOLUTE, 0.7f);

	// Open a log
	std::ofstream log_file;
	log_file.open(m_save_directory + "/weight_optimization_log.txt");

	std::ofstream cur_weights;

	float weights[24] = {0};

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
		double cur_factor = 0.1;
		while (new_score <= cur_score) {
			//Reset the weights to the original values
			for (int k = 0; k < NUM_SCALAR_FEATURES; k++) {
				m_queryEngine->TuneScalarWeights((SCALAR_FEATURES)k, weights[k]);
			}
			for (int k = 0; k < NUM_HISTOGRAM_FEATURES; k++) {
				m_queryEngine->TuneHistogramWeights((HISTOGRAM_FEATURES)k, weights[NUM_SCALAR_FEATURES + k]);
			}

			//Adjust scalar weights
			for (int k = 0; k < NUM_SCALAR_FEATURES; k++) {
				if (k == 5 || k == 14 || k == 16) { continue; } // Not using these features

				// with probability 10%, increase the weight, with probability 10% decrease the weight and with probability 80%, dont change
				int u = std::rand() % 2;
				if (u == 0) {
					m_queryEngine->TuneScalarWeights((SCALAR_FEATURES)k, m_queryEngine->m_scalar_weights[k] + cur_factor);
				}
				else if (u == 1) {
					m_queryEngine->TuneScalarWeights((SCALAR_FEATURES)k, std::max(0.0, m_queryEngine->m_scalar_weights[k] - cur_factor));
				}

			}

			// Compute histogram feature improvements
			for (int k = 0; k < NUM_HISTOGRAM_FEATURES; k++) {
				// with probability 10%, increase the weight, with probability 10% decrease the weight and with probability 80%, dont change
				int u = std::rand() % 2;
				if (u == 0) {
					m_queryEngine->TuneHistogramWeights((HISTOGRAM_FEATURES)k, m_queryEngine->m_scalar_weights[k] + cur_factor);
				}
				else if (u == 1) {
					m_queryEngine->TuneHistogramWeights((HISTOGRAM_FEATURES)k, std::max(0.0, m_queryEngine->m_scalar_weights[k] - cur_factor));
				}

			}
			

			// Compute the new score
			new_score = computeScore();
			cnt++;

			std::cout << "Iteration " << it << " count " << cnt << " attempts, computed score: " << new_score << std::endl;

			// Each time the counter reaches a multiple of 10, increase the factor by which we scale the weights
			if (cnt % 10 == 0) {
				cur_factor = std::max(0.05, cur_factor * 0.9);
			}

		}

		cur_score = new_score;
		std::cout << "Iteration " << it << ": Found improvement after " << cnt << " attempts, new score: " << new_score << std::endl;

		log_file << "Iteration " << it << ": Found improvement after " << cnt << " attempts, new score: " << new_score << std::endl;
		log_file << " Weights: ";
		//Save the new weights in the weights array and save them to file
		for (int k = 0; k < NUM_SCALAR_FEATURES; k++) {
			weights[k] = m_queryEngine->m_scalar_weights[k];
			log_file << m_queryEngine->m_scalar_weights[k] << ", ";

		}
		for (int k = 0; k < NUM_HISTOGRAM_FEATURES; k++) {
			weights[k + NUM_SCALAR_FEATURES] = m_queryEngine->m_histogram_weights[k];
			log_file << m_queryEngine->m_histogram_weights[k] << ", ";
		}
	
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
	// Used to obtain min, max, mean and std of histogram distances for normalization...

	m_queryEngine->LoadFeatures(m_feature_file);
	m_queryEngine->Initialize(STANDARDIZATION_TYPE::NO, STANDARDIZATION_TYPE::NO, DISTANCE_TYPE::EUCLIDEAN, hist_dist_type, 0);
	m_queryEngine->ComputeHistogramDistanceStatistics();
}