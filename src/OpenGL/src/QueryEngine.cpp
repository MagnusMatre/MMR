#include "QueryEngine.h"

QueryEngine::QueryEngine() {

}

QueryEngine::~QueryEngine() {

}

void QueryEngine::LoadFeatures(std::string& feature_file) {
	m_name_map.reserve(NUM_SHAPES);
	m_class_map.reserve(NUM_SHAPES);
	m_name_map.clear();
	m_class_map.clear();

	std::ifstream file(feature_file);
	std::string line = "";
	std::string cur_class_name = "";
	std::cout << "Start loading features..." << std::endl;
	for (int i = 0; i < NUM_SHAPES+1; i++) {
		std::getline(file, line);
		//std::cout << i << std::endl;
		if (i == 0) { continue; } // skip header
		std::istringstream iss(line);
		std::string token = "";
		for (int j = 0; j < NUM_FEATURES + 2; j++){
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
				m_features[i-1][j-2] = std::stof(token);
			}
		}
		//std::cout << "class map size: " << m_class_map.size() << std::endl;
		//std::cout << "name map size: " << m_name_map.size() << std::endl;
	}

	standardizeFeatures();
}

void QueryEngine::standardizeFeatures() {
	// Standardize the features
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

float QueryEngine::ComputeDistance(std::string& query_obj, std::string& other_obj) {
	// Print the size of the m_name_map

	int query_index = getIndex(query_obj);
	int other_index = getIndex(other_obj);

	float scalar_distance = 0.0f;
	for (int i = 0; i < NUM_SCALAR_FEATURES; i++) { // Only scalar features
		scalar_distance += m_scalar_weights[i] * (m_features[query_index][i] - m_features[other_index][i]) * (m_features[query_index][i] - m_features[other_index][i]);
	}
	scalar_distance = sqrt(scalar_distance);

	float hist_distance = 0.0f;
	for (int i = 0; i < NUM_HISTOGRAM_FEATURES; i++) { // Only histograms
		hist_distance += m_histogram_weights[i] * computeHistogramDistance(query_index, other_index, (HISTOGRAM_FEATURES)i);
	}
	hist_distance = sqrt(hist_distance);

	float distance = scalar_distance + hist_distance;

	//std::cout << "Distance between " << query_obj << " and " << other_obj << " is: " << distance << std::endl;
	return distance;
}

float QueryEngine::computeHistogramDistance(int i, int j, HISTOGRAM_FEATURES hist_type) {
	// For the positions we can just use i and j


	// The weights are histi and histj

	std::vector<float> posi;
	std::vector<float> posj;
	std::vector<float> histi;
	std::vector<float> histj;

	for (int k = m_histogram_ranges[hist_type].first; k < m_histogram_ranges[hist_type].second; k++) {
		posi.push_back(k - m_histogram_ranges[hist_type].first);
		posj.push_back(k - m_histogram_ranges[hist_type].first);
		histi.push_back(m_features[i][k]);
		histj.push_back(m_features[j][k]);
	}

	// Compute the EMD distance between the histograms using the functionality in the WassersteinEMD.h file
	return wasserstein(posi, histi, posj, histj);
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

void QueryEngine::ComputeFullDistanceMatrix() {
	for (int i = 0; i < NUM_SHAPES; i++) {
		for (int j = i; j < NUM_SHAPES; j++) {
			float distance = 0.0f;
			for (int k = 0; k < 13; k++) { // Only scalar features
				distance += (m_features[i][k] - m_features[j][k]) * (m_features[i][k] - m_features[j][k]);
			}
			distance = sqrt(distance);
			//m_distances[i][j] = distance;
			//m_distances[j][i] = distance;
		}
	}
}

void QueryEngine::TuneScalarWeights(SCALAR_FEATURES i, float weight) {
	m_scalar_weights[i] = weight;
}

void QueryEngine::TuneHistogramWeights(HISTOGRAM_FEATURES i, float weight) {
	m_histogram_weights[i] = weight;
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