#pragma once

#define NUM_QUERY_SHAPES 16
//#define NUM_QUERY_SHAPES 69
//#define NUM_QUERY_SHAPES 345
//#define NUM_QUERY_SHAPES 2283

#include "QueryEngine.h" 

#include <iostream>
#include <filesystem>
#include <memory>
#include <chrono>

class QueryBenchmark {
public:
	QueryBenchmark(std::string& feature_file, std::string& save_directory);
	~QueryBenchmark();

	void RunBenchmark(int K, STANDARDIZATION_TYPE s_type, STANDARDIZATION_TYPE s_type_histogram, DISTANCE_TYPE d_type_scalar, DISTANCE_TYPE d_type_histogram, float gamma);

	void RunBenchmarkScalarDistances();
	void RunBenchmarkHistogramDistances();
	void RunBenchmarkGamma();
	void RunBenchmarkScalarWeights_();
	void RunBenchmarkHistogramWeights_();
	void RunBenchmarkTiming(std::vector<int> K_values);

	void RunBenchmarkScalarWeights(int K, STANDARDIZATION_TYPE s_type, STANDARDIZATION_TYPE s_type_histogram, DISTANCE_TYPE d_type_scalar, DISTANCE_TYPE d_type_histogram, float gamma, std::vector<float> scalar_weights);
	void RunBenchmarkHistogramWeights(int K, STANDARDIZATION_TYPE s_type, STANDARDIZATION_TYPE s_type_histogram, DISTANCE_TYPE d_type_scalar, DISTANCE_TYPE d_type_histogram, float gamma, std::vector<float> histogram_weights);

	void ComputeDistanceStats(DISTANCE_TYPE hist_dist_type);

	void WeightOptimizer(int max_iterations);

	void ComputeDistanceMatrix();

private:
	std::unique_ptr<QueryEngine> m_queryEngine;

	std::string m_feature_file;
	std::string m_save_root;

	std::string m_save_directory;

	// one random per 16 classes
	int m_queryIndices[NUM_QUERY_SHAPES] = { 2182,911,730,1317,2221,1660,1284,1963,1758,55,1360,1707,1119,1918,786,2118 };

	// one random shape per class
	//int m_queryIndices[NUM_QUERY_SHAPES] = { 0, 26, 40, 63, 92, 124, 178, 210, 232, 258, 290, 356, 392, 397, 443, 461, 502, 519, 527, 555, 587, 628, 648, 667, 694, 705, 728, 772, 825, 840, 895, 925, 935, 1086, 1172, 1245, 1306, 1334, 1355, 1380, 1391, 1433, 1465, 1498, 1534, 1573, 1590, 1658, 1707, 1756, 1779, 1809, 1856, 1918, 1934, 1945, 1970, 1983, 2008, 2049, 2098, 2128, 2166, 2177, 2194, 2214, 2239, 2248, 2272 };

	//int m_queryIndices[NUM_QUERY_SHAPES] = { 0, 2, 8, 1, 10, 26, 16, 15, 25, 22, 40, 31, 35, 33, 54, 63, 62, 76, 64, 77, 92, 98, 90, 88, 109, 124, 142, 134, 144, 150, 178, 179, 185, 198, 174, 210, 205, 206, 213, 209, 232, 223, 216, 217, 230, 258, 245, 280, 260, 275, 290, 283, 295, 287, 291, 356, 303, 351, 363, 339, 392, 378, 380, 395, 381, 397, 405, 423, 402, 425, 443, 456, 454, 455, 458, 461, 459, 471, 463, 462, 502, 495, 496, 484, 488, 519, 518, 508, 505, 520, 527, 548, 541, 526, 523, 555, 557, 575, 561, 577, 587, 592, 596, 600, 584, 628, 605, 632, 637, 617, 648, 655, 645, 640, 642, 667, 665, 668, 672, 671, 694, 701, 683, 699, 679, 705, 717, 707, 704, 715, 728, 739, 740, 731, 732, 772, 765, 792, 746, 753, 825, 809, 807, 815, 805, 840, 848, 842, 845, 838, 895, 863, 877, 855, 888, 925, 923, 929, 932, 904, 935, 958, 960, 940, 950, 1086, 1077, 1103, 1053, 989, 1172, 1170, 1148, 1162, 1132, 1245, 1183, 1278, 1230, 1284, 1306, 1321, 1316, 1308, 1307, 1334, 1329, 1343, 1338, 1326, 1355, 1352, 1354, 1347, 1364, 1380, 1373, 1377, 1374, 1371, 1391, 1390, 1386, 1394, 1405, 1433, 1435, 1428, 1421, 1429, 1465, 1476, 1471, 1441, 1462, 1498, 1486, 1490, 1500, 1491, 1534, 1531, 1512, 1540, 1527, 1573, 1570, 1574, 1572, 1561, 1590, 1586, 1630, 1623, 1584, 1658, 1637, 1659, 1640, 1650, 1707, 1692, 1686, 1677, 1674, 1756, 1724, 1723, 1759, 1778, 1779, 1781, 1780, 1790, 1792, 1809, 1816, 1801, 1827, 1802, 1856, 1847, 1831, 1848, 1853, 1918, 1895, 1922, 1897, 1871, 1934, 1929, 1937, 1940, 1932, 1945, 1950, 1948, 1946, 1944, 1970, 1961, 1958, 1967, 1962, 1983, 1974, 1975, 1999, 1980, 2008, 2019, 2002, 2014, 2010, 2049, 2046, 2069, 2057, 2063, 2098, 2097, 2087, 2075, 2074, 2128, 2138, 2125, 2127, 2123, 2166, 2161, 2158, 2156, 2142, 2177, 2186, 2178, 2170, 2185, 2194, 2192, 2195, 2205, 2193, 2214, 2227, 2222, 2223, 2210, 2239, 2228, 2229, 2236, 2232, 2248, 2258, 2260, 2257, 2259, 2272, 2280, 2276, 2274, 2275 };

	float m_frequencies[NUM_QUERY_SHAPES] = { 0.0f };


	double computeScore();
	void getFrequencies();

};

/* output of random selection of 16 objects

Index	    ClassName			Filename
2182		Truck				D00518.obj
911			House				D00965.obj
730			Guitar				D00232.obj
1317		Knife				D01189.obj
2221		Vase				m541.obj
1660		PlantWildNonTree	m987.obj
1284		Jet					m1283.obj
1963		Spoon				D00517.obj
1758		RectangleTable		m891.obj
55			AquaticAnimal		m78.obj
1360		Monitor				D00915.obj
1707		Quadruped			m88.obj
1119		Insect				D00102.obj
1918		Ship				m1460.obj
786			Gun					m672.obj
2118		Train				D01016.obj

*/