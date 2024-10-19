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
	for (int i = 0; i < NUM_SHAPES+1; i++) {
		std::getline(file, line);
		std::cout << i << std::endl;
		if (i == 0) { continue; } // skip header
		std::istringstream iss(line);
		std::string token = "";
		for (int j = 0; j < NUM_FEATURES + 2;j++){
			std::getline(iss, token, '\t');
			if (j == 0) {
				m_class_map.push_back(token);
				cur_class_name = token;
			}
			else if (j == 1) {
				std::string cur_shape_name = cur_class_name + "/" + token;
				std::cout << i << " " << cur_shape_name << std::endl;
				m_name_map.push_back(cur_shape_name);
			}
			else {
				//std::cout << "Token " << "(i,j): " << i << " " << j << " " << token << std::endl;
				m_features[i-1][j-2] = std::stof(token);
			}
		}
		std::cout << "class map size: " << m_class_map.size() << std::endl;
		std::cout << "name map size: " << m_name_map.size() << std::endl;
	}
}

void QueryEngine::ComputeDistance(std::string& query_obj, std::string& other_obj) {
	// Print the size of the m_name_map

	float distance = 0.0f;
	/*for (int i = 0; i < NUM_FEATURES; i++) {
		distance += (m_features[query_obj_index][i] - m_features[other_obj_index][i]) * (m_features[query_obj_index][i] - m_features[other_obj_index][i]);
	}*/
	int query_index = getIndex(query_obj);
	int other_index = getIndex(other_obj);

	distance += (m_features[query_index][0] - m_features[other_index][0]) * (m_features[query_index][0] - m_features[other_index][0]);
	distance = sqrt(distance);
	std::cout << "Distance between " << query_obj << " and " << other_obj << " is: " << distance << std::endl;
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