#include "QueryEngine.h"

QueryEngine::QueryEngine() {

}

QueryEngine::~QueryEngine() {

}

void QueryEngine::Initialize(STANDARDIZATION_TYPE s_type, STANDARDIZATION_TYPE s_type_histogram, DISTANCE_TYPE d_type_scalar, DISTANCE_TYPE d_type_histogram, float gamma) {
	standardizeFeatures(s_type);
	m_histogram_distance_standardization = s_type_histogram;
	m_distance_type_scalar = d_type_scalar;
	m_distance_type_histogram = d_type_histogram;
	m_gamma = gamma;

	std::cout << "Finished initializing QueryEngine with (scalar_norm, histogram_norm, scalar_distance, histogram_distsance, gamma) = (" <<
		s_type << "," << s_type_histogram << "," << d_type_scalar << "," << d_type_histogram << "," << gamma << ")" << std::endl;
}

void QueryEngine::LoadFeatures(std::string& feature_file) {
	//std::cout << "Start loading features..." << std::endl;
	m_name_map.clear();
	m_class_map.clear();
	m_name_map.reserve(NUM_SHAPES);
	m_class_map.reserve(NUM_SHAPES);

	std::ifstream file(feature_file);
	std::string line = "";
	std::string cur_class_name = "";
	for (int i = 0; i < NUM_SHAPES + 1; i++) {
		std::getline(file, line);
		//std::cout << i << std::endl;
		if (i == 0) { continue; } // skip header
		std::istringstream iss(line);
		std::string token = "";
		for (int j = 0; j < NUM_FEATURES + 2; j++) {
			std::getline(iss, token, '\t');
			if (j == 0) {
				m_class_map.push_back(token);
				cur_class_name = token;
			}
			else if (j == 1) {
				std::string cur_shape_name = cur_class_name + "/" + token;
				//std::cout << i << " " << cur_shape_name << std::endl;
				m_name_map.push_back(cur_shape_name);
			}
			else {
				//std::cout << "Token " << "(i,j): " << i << " " << j << " " << token << std::endl;
				m_features[i - 1][j - 2] = std::stof(token);
			}
		}
		//std::cout << "class map size: " << m_class_map.size() << std::endl;
		//std::cout << "name map size: " << m_name_map.size() << std::endl;
	}
}

void QueryEngine::standardizeFeatures(STANDARDIZATION_TYPE s_type) {
	// Standardize the features
	if (s_type == STANDARDIZATION_TYPE::STANDARD) {
		for (int i = 0; i < NUM_SCALAR_FEATURES; i++) {
			float mean = 0.0f;
			float std = 0.0f;
			for (int j = 0; j < NUM_SHAPES; j++) {
				mean += m_features[j][i];
			}
			mean /= NUM_SHAPES;
			for (int j = 0; j < NUM_SHAPES; j++) {
				std += (m_features[j][i] - mean) * (m_features[j][i] - mean);
			}
			std = sqrt(std / NUM_SHAPES);
			for (int j = 0; j < NUM_SHAPES; j++) {
				m_features[j][i] = (m_features[j][i] - mean) / std;
			}
		}
	}
	else if (s_type == STANDARDIZATION_TYPE::RANGE) {
		for (int i = 0; i < NUM_SCALAR_FEATURES; i++) {
			float min = 1000000.0f;
			float max = -1000000.0f;
			for (int j = 0; j < NUM_SHAPES; j++) {
				if (m_features[j][i] < min) {
					min = m_features[j][i];
				}
				if (m_features[j][i] > max) {
					max = m_features[j][i];
				}
			}
			for (int j = 0; j < NUM_SHAPES; j++) {
				m_features[j][i] = (m_features[j][i] - min) / (max - min);
			}
		}
	}
	else {
		return;
	}
}

void QueryEngine::ComputeHistogramDistanceStatistics() {
	// Iterate over all pairs of histograms and compute the mean, standard deviation, min and max

	double histogram_means[NUM_HISTOGRAM_FEATURES] = { 0.0 };
	double histogram_stds[NUM_HISTOGRAM_FEATURES] = { 0.0 };
	double histogram_mins[NUM_HISTOGRAM_FEATURES] = { 1000000.0 };
	double histogram_maxs[NUM_HISTOGRAM_FEATURES] = { -1000000.0 };

	int32_t cnt = 0;
	//for (int i = 0; i < NUM_SHAPES; i++) {
	for (int i = 0; i < NUM_SHAPES; i++) {
		//if (m_distance_type_histogram == DISTANCE_TYPE::EMD && rand() % 100 < 95) { continue; } // Only use 5% of the data (for now
		std::cout << i << "/" << NUM_SHAPES << ": " << histogram_means[0] << " " << histogram_stds[1] << std::endl;
		for (int j = i + 1; j < NUM_SHAPES; j++) {
			for (int k = 0; k < NUM_HISTOGRAM_FEATURES; k++) {

				// Update the values
				double cur_mean = histogram_means[k];
				double cur_std = histogram_stds[k];

				// Compute the distance between the histograms of i and j
				double distance = computeHistogramDistance(i, j, (HISTOGRAM_FEATURES)k);

				if (cnt == 0) {
					histogram_means[k] = distance;
					histogram_stds[k] = 0.0;
					histogram_mins[k] = distance;
					histogram_maxs[k] = distance;
				}
				else {
					histogram_means[k] = (cur_mean * cnt + distance) / (cnt + 1);
					histogram_stds[k] = (((double)cnt - 1) / (double)cnt) * cur_std + (distance - cur_mean) * (distance - cur_mean) / ((double)cnt + 1);
					if (distance < histogram_mins[k]) {
						histogram_mins[k] = distance;
					}
					if (distance > histogram_maxs[k]) {
						histogram_maxs[k] = distance;
					}
				}
			}
			cnt++;
		}
	}

	// Open file
	std::ofstream results_file;
	std::string save_name = "res/distance_stats/" + std::to_string(m_distance_type_histogram) + ".txt";
	results_file.open(save_name);

	std::cout << "Histogram statistics: " << std::endl;
	results_file << "Histogram statistics" << std::endl;
	std::cout << "{ MIN, MAX, MEAN, STD}" << std::endl;
	results_file << "{ MIN, MAX, MEAN, STD}" << std::endl;

	for (int i = 0; i < NUM_HISTOGRAM_FEATURES; i++) {
		histogram_stds[i] = sqrt(histogram_stds[i] / cnt);
		std::cout << "Histogram feature " << i << ": " << "{" << histogram_mins[i] << ", " << histogram_maxs[i] << ", " << histogram_means[i] << ", " << std::sqrt(histogram_stds[i]) << "}" << std::endl;
		results_file << "Histogram feature " << i << ": " << "{" << histogram_mins[i] << ", " << histogram_maxs[i] << ", " << histogram_means[i] << ", " << std::sqrt(histogram_stds[i]) << "}" << std::endl;
	}
	results_file.close();

}

double QueryEngine::ComputeDistance(std::string& query_obj, std::string& other_obj) {
	// Print the size of the m_name_map

	int query_index = getIndex(query_obj);
	int other_index = getIndex(other_obj);

	double scalar_distance = computeScalarDistance(query_index, other_index);

	double hist_distance = 0.0f;
	for (int i = 0; i < NUM_HISTOGRAM_FEATURES; i++) { // Only histograms
		hist_distance += m_histogram_weights[i] * computeHistogramDistance(query_index, other_index, (HISTOGRAM_FEATURES)i);
	}

	double distance = m_gamma * scalar_distance + (1 - m_gamma) * hist_distance;

	//std::cout << "Distance between " << query_obj << " and " << other_obj << " is: " << distance << std::endl;
	return distance;
}

double QueryEngine::ComputeDistance(int query_obj, int other_obj) {
	// Print the size of the m_name_map

	double scalar_distance = computeScalarDistance(query_obj, other_obj);

	double hist_distance = 0.0f;
	float hist_weight_sum = getHistogramWeightsSum();
	for (int i = 0; i < NUM_HISTOGRAM_FEATURES; i++) { // Only histograms
		float cur_weight = (1-m_gamma) * m_histogram_weights[i] / hist_weight_sum;
		hist_distance += cur_weight * computeHistogramDistance(query_obj, other_obj, (HISTOGRAM_FEATURES)i);
	}

	double distance = scalar_distance + hist_distance;

	//std::cout << "Distance between " << query_obj << " and " << other_obj << " is: " << distance << std::endl;
	return distance;
}

double QueryEngine::computeScalarDistance(int i, int j) {
	float weight_sum = getScalarWeightsSum();
	if (m_distance_type_scalar == DISTANCE_TYPE::MYABSOLUTE) {
		float distance = 0.0f;
		for (int k = 0; k < NUM_SCALAR_FEATURES; k++) { // Only scalar 
			float cur_weight = m_gamma * m_scalar_weights[k] / weight_sum;
			distance += cur_weight * abs(m_features[i][k] - m_features[j][k]);
		}
		return distance;
	}
	else if (m_distance_type_scalar == DISTANCE_TYPE::EUCLIDEAN) {
		float distance = 0.0f;
		for (int k = 0; k < NUM_SCALAR_FEATURES; k++) { // Only scalar features
			float cur_weight = m_gamma * m_scalar_weights[k] / weight_sum;
			distance += cur_weight * (m_features[i][k] - m_features[j][k]) * (m_features[i][k] - m_features[j][k]);
		}
		return sqrt(distance);
	}
	else if (m_distance_type_scalar == DISTANCE_TYPE::ONEPOINTFIVE) { // 1.5 norm
		float distance = 0.0f;
		for (int k = 0; k < NUM_SCALAR_FEATURES; k++) { // Only scalar features
			float cur_weight = m_gamma * m_scalar_weights[k] / weight_sum;
			distance += cur_weight * pow(abs(m_features[i][k] - m_features[j][k]), 1.5);
		}
		return distance;
	}
	else if (m_distance_type_scalar == DISTANCE_TYPE::COSINE) { // scaling does not make sense? Whole distance does not make sense for scalar part
		double dot_product = 0.0f;
		double norm_i = 0.0f;
		double norm_j = 0.0f;
		for (int k = 0; k < NUM_SCALAR_FEATURES; k++) {
			dot_product += m_features[i][k] * m_features[j][k];
			norm_i += m_features[i][k] * m_features[i][k];
			norm_j += m_features[j][k] * m_features[j][k];
		}
		norm_i = sqrt(norm_i);
		norm_j = sqrt(norm_j);
		double true_distance = 1.0f - dot_product / (norm_i * norm_j);
		return true_distance;
	}
	else {
		return 0.0f;
	}
}

double QueryEngine::computeHistogramDistance(int i, int j, HISTOGRAM_FEATURES hist_type) {
	// For the positions we can just use i and j

	// The weights are histi and histj

	if (m_distance_type_histogram == DISTANCE_TYPE::EMD) {
		std::vector<double> posi;
		std::vector<double> posj;
		std::vector<double> histi;
		std::vector<double> histj;

		for (int k = m_histogram_ranges[hist_type].first; k < m_histogram_ranges[hist_type].second; k++) {
			posi.push_back(k - m_histogram_ranges[hist_type].first);
			posj.push_back(k - m_histogram_ranges[hist_type].first);
			histi.push_back(m_features[i][k]);
			histj.push_back(m_features[j][k]);
		}

		double true_distance = wasserstein(posi, histi, posj, histj);
		double distance = standardizeHistogramDistance(true_distance, hist_type);

		// Compute the EMD distance between the histograms using the functionality in the WassersteinEMD.h file
		return distance;
	}

	else if (m_distance_type_histogram == DISTANCE_TYPE::COSINE) {
		double dot_product = 0.0f;
		double norm_i = 0.0f;
		double norm_j = 0.0f;
		for (int k = m_histogram_ranges[hist_type].first; k < m_histogram_ranges[hist_type].second; k++) {
			dot_product += m_features[i][k] * m_features[j][k];
			norm_i += m_features[i][k] * m_features[i][k];
			norm_j += m_features[j][k] * m_features[j][k];
		}
		norm_i = sqrt(norm_i);
		norm_j = sqrt(norm_j);
		double true_distance = 1.0f - dot_product / (norm_i * norm_j);
		return standardizeHistogramDistance(true_distance, hist_type);
	}

	else if (m_distance_type_histogram == DISTANCE_TYPE::EUCLIDEAN) {
		double distance = 0.0f;
		for (int k = m_histogram_ranges[hist_type].first; k < m_histogram_ranges[hist_type].second; k++) {
			distance += (m_features[i][k] - m_features[j][k]) * (m_features[i][k] - m_features[j][k]);

		}
		double true_distance = sqrt(distance);
		return standardizeHistogramDistance(true_distance, hist_type);
	}
	else if (m_distance_type_histogram == DISTANCE_TYPE::MYABSOLUTE) {
		double distance = 0.0f;
		for (int k = m_histogram_ranges[hist_type].first; k < m_histogram_ranges[hist_type].second; k++) {
			distance += abs(m_features[i][k] - m_features[j][k]);
		}
		return standardizeHistogramDistance(distance, hist_type);
	}
	else {
		return 0.0;
	}

}

double QueryEngine::standardizeHistogramDistance(double true_distance, HISTOGRAM_FEATURES k) {
	double mean;
	double std;
	double min;
	double max;
	if (m_distance_type_histogram == DISTANCE_TYPE::EUCLIDEAN) {
		min = hist_euclidean_min_max_mu_std[k][0];
		max = hist_euclidean_min_max_mu_std[k][1];
		mean = hist_euclidean_min_max_mu_std[k][2];
		std = hist_euclidean_min_max_mu_std[k][3];
	}
	else if (m_distance_type_histogram == DISTANCE_TYPE::COSINE) {
		min = hist_cosine_min_max_mu_std[k][0];
		max = hist_cosine_min_max_mu_std[k][1];
		mean = hist_cosine_min_max_mu_std[k][2];
		std = hist_cosine_min_max_mu_std[k][3];
	}
	else if (m_distance_type_histogram == DISTANCE_TYPE::MYABSOLUTE) {
		min = hist_absolute_min_max_mu_std[k][0];
		max = hist_absolute_min_max_mu_std[k][1];
		mean = hist_absolute_min_max_mu_std[k][2];
		std = hist_absolute_min_max_mu_std[k][3];
	}
	else if (m_distance_type_histogram == DISTANCE_TYPE::EMD) {
		min = hist_emd_min_max_mu_std[k][0];
		max = hist_emd_min_max_mu_std[k][1];
		mean = hist_emd_min_max_mu_std[k][2];
		std = hist_emd_min_max_mu_std[k][3];
	}
	else {
		return true_distance;
	}

	if (m_histogram_distance_standardization == STANDARDIZATION_TYPE::STANDARD) {

		return (true_distance - mean) / std;
	}
	else if (m_histogram_distance_standardization == STANDARDIZATION_TYPE::RANGE) {
		return (true_distance - min) / (max - min);
	}
	else {
		return true_distance;
	}
}


int QueryEngine::getIndex(std::string name_obj) {
	// Find the index of the name using binary search on m_name_map

	int l, r, m;
	l = 0;
	r = NUM_SHAPES - 1;
	while (l <= r) {
		int m = l + (r - l) / 2;
		//std::cout << "m: " << m << std::endl;
		// Check if x is present at mid
		//std::cout << m_name_map[m] << std::endl;
		//std::cout << name_obj << std::endl;
		int cmp = m_name_map[m].compare(name_obj);
		if (cmp == 0)
			return m;

		// If x greater, ignore left half
		if (cmp < 0)
			l = m + 1;

		// If x is smaller, ignore right half
		else
			r = m;
	}
}

std::string QueryEngine::getObjectName(int index) {
	return m_name_map[index];
}

std::string QueryEngine::getClassName(int index) {
	return m_class_map[index];
}

int QueryEngine::getFrequency(int index) {
	int cur_freq = 0;
	for (int k = 0; k < 69; k++) {
		if (cur_freq + m_frequencies[k] > index) {
			return m_frequencies[k];
		}
		cur_freq += m_frequencies[k];
	}
	return m_frequencies[68];
}

void QueryEngine::ComputeFullDistanceMatrix() {
	// Open file
	std::ofstream results_file;
	std::string save_name = "../../res/distance_matrix/" + std::to_string(m_distance_type_scalar) + "_" + std::to_string(m_distance_type_histogram) + "_" + std::to_string(m_gamma) + ".txt";

	results_file.open(save_name);

	for (int i = 0; i < NUM_SHAPES; i++) {
		std::cout << "i " << i << std::endl;
		for (int j = 0; j < NUM_SHAPES; j++) {
			
			float distance = ComputeDistance(i, j);

			results_file << distance << ",";
			//m_distances[i][j] = distance;
			//m_distances[j][i] = distance;
		}
		results_file << std::endl;
	}

	results_file.close();


}

void QueryEngine::TuneScalarWeights(SCALAR_FEATURES i, float weight) {
	m_scalar_weights[i] = weight;
}

void QueryEngine::TuneHistogramWeights(HISTOGRAM_FEATURES i, float weight) {
	m_histogram_weights[i] = weight;
}

void QueryEngine::TuneGamma(float gamma) {
	m_gamma = gamma;
}

float QueryEngine::getGamma() {
	return m_gamma;
}

float QueryEngine::getScalarWeightsSum() {
	float sum = 0.0f;
	for (int i = 0; i < NUM_SCALAR_FEATURES; i++) {
		sum += m_scalar_weights[i];
	}
	return sum;
}

float QueryEngine::getHistogramWeightsSum() {
	float sum = 0.0f;
	for (int i = 0; i < NUM_HISTOGRAM_FEATURES; i++) {
		sum += m_histogram_weights[i];
	}
	return sum;
}

void QueryEngine::SaveDistanceMatrix(std::string& save_path) {
	std::ofstream file(save_path);
	for (int i = 0; i < NUM_SHAPES; i++) {
		for (int j = 0; j < NUM_SHAPES; j++) {
			//file << m_distances[i][j] << ",";
		}
		file << std::endl;
	}
	file.close();

}

void QueryEngine::LoadDistanceMatrix(std::string& load_path) {
	std::ifstream file(load_path);
	std::string line = "";
	int i = 0;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string token = "";
		for (int j = 0; j < NUM_SHAPES; j++) {
			std::getline(iss, token, ',');
			//m_distances[i][j] = std::stof(token);
		}
		i++;
	}
}

std::vector<std::pair<float, std::string>> QueryEngine::DoBenchmark(int query_obj, int K) {
	std::vector<std::pair<float, std::string>> results;
	std::vector<std::pair<float, int>> distances;


	for (int i = 0; i < NUM_SHAPES; i++) {
		if (i == query_obj) {
			continue;
		}
		distances.push_back(std::make_pair(ComputeDistance(query_obj, i), i));
	}
	std::sort(distances.begin(), distances.end());
	for (int i = 0; i < K; i++) {
		results.push_back(std::make_pair(distances[i].first, getObjectName(distances[i].second)));
	}
	return results;
}

std::vector<std::pair<float, std::string>> QueryEngine::DoBenchmarkTiming(int query_obj, int K, double& total_time) {
	auto start = std::chrono::high_resolution_clock::now();

	std::vector<std::pair<float, std::string>> results;
	std::vector<std::pair<float, int>> distances;

	for (int i = 0; i < NUM_SHAPES; i++) {
		if (i == query_obj) {
			continue;
		}
		distances.push_back(std::make_pair(ComputeDistance(query_obj, i), i));
	}
	std::sort(distances.begin(), distances.end());
	for (int i = 0; i < K; i++) {
		results.push_back(std::make_pair(distances[i].first, getObjectName(distances[i].second)));
	}
	auto end = std::chrono::high_resolution_clock::now();

	total_time = std::chrono::duration<double>(end - start).count();

	return results;
}