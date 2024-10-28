#pragma once

#include "QueryEngine.h" 

#include <iostream>
#include <filesystem>
#include <memory>

class QueryBenchmark {
	public:
		QueryBenchmark(std::string& feature_file, std::string& save_directory);
		~QueryBenchmark();

		void RunBenchmark(int K, STANDARDIZATION_TYPE s_type, STANDARDIZATION_TYPE s_type_histogram, DISTANCE_TYPE d_type_scalar, DISTANCE_TYPE d_type_histogram, float gamma);

		void ComputeDistanceStats(DISTANCE_TYPE hist_dist_type);

		void WeightOptimizer(int max_iterations);

	private:
		std::unique_ptr<QueryEngine> m_queryEngine;

		std::string m_feature_file;
		std::string m_save_directory;

		int m_queryIndices[16] = {2182,911,730,1317,2221,1660,1284,1963,1758,55,1360,1707,1119,1918,786,2118};

		float m_frequencies[16] = { 0.0f };

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