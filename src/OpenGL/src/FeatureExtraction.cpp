#include "FeatureExtraction.h"

FeatureExtraction::FeatureExtraction() {
}

FeatureExtraction::~FeatureExtraction() {
}

void FeatureExtraction::Load(std::string& mesh_path, std::string& convex_hull_path, std::string& samplepoints_file_name) {
	
	load_mesh(mesh_path, m_mesh);
	load_mesh(convex_hull_path, m_convexhull);
	load_samplepoints(samplepoints_file_name);
}

void FeatureExtraction::load_mesh(std::string& filename, CGALMesh& m_mesh) {
	if (!CGALPMP::IO::read_polygon_mesh(filename, m_mesh)) {
		std::cerr << "Error: cannot read file " << filename << std::endl;
		return;
	}

	if (is_empty(m_mesh))
	{
		std::cerr << "Warning: empty file?" << std::endl;
		return;
	}
}

void FeatureExtraction::compute_convex_hull() {
	// Compute the convex hull using QHull (see https://stackoverflow.com/questions/19530731/qhull-library-c-interface)

	std::vector<double> points_3D = {};

	// Push the points of the mesh into the points_3D vector
	for (auto v : m_mesh.vertices()) {
		points_3D.push_back(m_mesh.point(v)[0]);
		points_3D.push_back(m_mesh.point(v)[1]);
		points_3D.push_back(m_mesh.point(v)[2]);
	}

	int ndim = 3;
	int num_points = points_3D.size() / ndim;

	std::string comment = ""; // rbox commands, see http://www.qhull.org/html/rbox.htm
	std::string qhull_command = ""; // For qhull commands, see http://www.qhull.org/html/qhull.htm

	orgQhull::Qhull qhull = orgQhull::Qhull(comment.c_str(), ndim, num_points, points_3D.data(), qhull_command.c_str());
	//std::cout << "qhull.hullDimension(): " << qhull.hullDimension() << "\n";
	//std::cout << "qhull.volume(): " << qhull.volume() << "\n";
	//std::cout << "qhull.area(): " << qhull.area() << "\n";

	m_hull_volume = qhull.volume();
	m_hull_surface_area = qhull.area();
}

void FeatureExtraction::compute_all_features() {
	compute_convex_hull();
	get_diameter();
	//get_bounding_box_statistics();
	get_volume();
	//get_concomp_volume();
	//get_surface_area();

	//get_compactness(); // Needs to be done after volume and surface area
	//get_sphericity(); // Needs to be done after compactness

	//get_rectangularity(); // Needs to be done after bounding box statistics

	//get_eccentricities();
	get_convexity();

	//get_A3_histogram();
	//get_D1_histogram();
	//get_D2_histogram();
	//get_D3_histogram();
	//get_D4_histogram();
}

void FeatureExtraction::print_all_features() {
	std::cout << "Surface area:				 " << m_surface_area << std::endl;
	std::cout << "Volume:					 " << m_volume << std::endl;
	std::cout << "Volume_comps:				 " << m_volume2 << std::endl;
	std::cout << "Diameter:					 " << m_diameter << std::endl;
	std::cout << "Bounding box diameter:\t\t " << m_bounding_box_diameter << std::endl;
	std::cout << "Bounding box volume:\t\t	 " << m_bounding_box_volume << std::endl;
	std::cout << "Convexity:				 " << m_convexity << std::endl;
	std::cout << "Eccentricity02:			 " << m_eccentricity02 << std::endl;
	std::cout << "Eccentricity01:			 " << m_eccentricity01 << std::endl;
	std::cout << "Eccentricity12:			 " << m_eccentricity12 << std::endl;
	std::cout << "Compactness:				 " << m_compactness << std::endl;
	std::cout << "Sphericity:				 " << m_sphericity << std::endl;
	std::cout << "Rectangularity:\t			 " << m_rectangularity << std::endl;

	std::cout << "A3 Bin values: [";
	for (int i = 0; i < NUM_A3_BINS - 1; i++) {
		std::cout << A3_bins[i] << ", ";
	}
	std::cout << A3_bins[NUM_A3_BINS - 1] << "]" << std::endl;

	std::cout << "D1 Bin values: [";
	for (int i = 0; i < NUM_D1_BINS - 1; i++) {
		std::cout << D1_bins[i] << ", ";
	}
	std::cout << D1_bins[NUM_D1_BINS - 1] << "]" << std::endl;

	std::cout << "D2 Bin values: [";
	for (int i = 0; i < NUM_D2_BINS - 1; i++) {
		std::cout << D2_bins[i] << ", ";
	}
	std::cout << D2_bins[NUM_D2_BINS - 1] << "]" << std::endl;

	std::cout << "D3 Bin values: [";
	for (int i = 0; i < NUM_D3_BINS - 1; i++) {
		std::cout << D3_bins[i] << ", ";
	}
	std::cout << D3_bins[NUM_D3_BINS - 1] << "]" << std::endl;

	std::cout << "D4 Bin values: [";
	for (int i = 0; i < NUM_D4_BINS - 1; i++) {
		std::cout << D4_bins[i] << ", ";
	}
	std::cout << D4_bins[NUM_D4_BINS - 1] << "]" << std::endl;

}

std::string FeatureExtraction::get_feature_string() {
	std::string feature_string =      std::to_string(m_diameter) + "\t"
									+ std::to_string(m_bounding_box_diameter) + "\t"
									+ std::to_string(m_bounding_box_volume) + "\t"
									+ std::to_string(m_surface_area) + "\t"
									+ std::to_string(m_volume) + "\t"
									+ std::to_string(m_volume2) + "\t"
									+ std::to_string(m_convexity) + "\t"
									+ std::to_string(m_eccentricity02) + "\t"
									+ std::to_string(m_eccentricity01) + "\t"
									+ std::to_string(m_eccentricity12) + "\t"
									+ std::to_string(m_compactness) + "\t"
									+ std::to_string(m_sphericity) + "\t"
									+ std::to_string(m_rectangularity);

	for (int i = 0; i < NUM_A3_BINS; i++) {
		feature_string += "\t" + std::to_string(A3_bins[i]);
	}
	for (int i = 0; i < NUM_D1_BINS; i++) {
		feature_string += "\t" + std::to_string(D1_bins[i]);
	}
	for (int i = 0; i < NUM_D2_BINS; i++) {
		feature_string += "\t" + std::to_string(D2_bins[i]);
	}
	for (int i = 0; i < NUM_D3_BINS; i++) {
		feature_string += "\t" + std::to_string(D3_bins[i]);
	}
	for (int i = 0; i < NUM_D4_BINS; i++) {
		feature_string += "\t" + std::to_string(D4_bins[i]);
	}

	return feature_string;

}

void FeatureExtraction::get_surface_area() {
	// Iterate over all triangles in the mesh and compute its area

	double A = 0.0;

	// Iterate over all the triangles in the mesh
	for (auto f : m_mesh.faces()) {
		auto h = m_mesh.halfedge(f);
		auto v1 = m_mesh.point(m_mesh.target(h));
		auto v2 = m_mesh.point(m_mesh.target(m_mesh.next(h)));
		auto v3 = m_mesh.point(m_mesh.target(m_mesh.next(m_mesh.next(h))));

		// Compute the area of the triangle by using the cross product
		A += 0.5 * std::sqrt(CGAL::cross_product(v2 - v1, v3 - v1).squared_length());
		
	}
	std::cout << "My area: " << A << std::endl;
	std::cout << "CGAL area: " << CGAL::Polygon_mesh_processing::area(m_mesh) << std::endl;

	m_surface_area = A;
}

void FeatureExtraction::get_volume() {
	// V = (1/6) * | sum_{t} (v1 x v2) . v3 |
	// where v1=x1-o, v2=x2-o and v3=x3-o are the vectors of the triangle t, and o is the origin (aka barycenter)

	double V = 0.0;
	double V_CGAL = 0.0;


	// If the mesh is closed, compute area with CGAL
	if (CGAL::is_closed(m_mesh)) {
		m_volume = CGAL::Polygon_mesh_processing::volume(m_mesh);
		return;
	}

	// Otherwise, use own area computation method... (maybe always use our own method? Dependends on how many meshes are closed...)

	Point_3 barycenter(0.0, 0.0, 0.0);

	// Iterate over all the triangles in the mesh
	for (auto f : m_mesh.faces()) {
		auto h = m_mesh.halfedge(f);
		auto v1 = m_mesh.point(m_mesh.target(h)) - barycenter;
		auto v2 = m_mesh.point(m_mesh.target(m_mesh.next(h))) - barycenter;
		auto v3 = m_mesh.point(m_mesh.target(m_mesh.next(m_mesh.next(h)))) - barycenter;

		// Compute the volume of the tetrahedron
		double volume = CGAL::cross_product(v1, v2) * v3;
		V += volume;
	}

	std::cout << "My volume: " << std::abs(V)/6.0 << std::endl;
	//std::cout << "CGAL volume: " << CGAL::Polygon_mesh_processing::volume(m_mesh) << std::endl;

	m_volume = std::abs(V) / 6.0;

}

void FeatureExtraction::get_concomp_volume(){
	// Compute the volume of the connected components of the mesh
	// This is useful for meshes that are not closed

	// Compute the connected components of the mesh
	CGALMesh::Property_map<face_descriptor, std::size_t> fccmap = m_mesh.add_property_map<face_descriptor, std::size_t>("f:CC").first;
	std::size_t num = CGAL::Polygon_mesh_processing::connected_components(m_mesh, fccmap);

	typedef std::map<std::size_t/*index of CC*/, unsigned int/*nb*/> Components_size;
	Components_size nb_per_cc;
	for (face_descriptor f : faces(m_mesh)) {
		nb_per_cc[fccmap[f]]++;
	}

	m_num_comps = num;

	std::cout << "Number of comps: " << num << std::endl;

	double V = 0.0;

	std::map<std::size_t, Point_3> barycenters; // Maps each index of CC to its barycenter
	std::map<std::size_t, double> volumes;  // Maps index of CC to its volume

	//Initialize varycenters
	for (std::size_t i = 0; i < num; i++) {
		barycenters[i] = Point_3(0.0, 0.0, 0.0);
	}

	// Compute barycenters of CCs
	for (face_descriptor f : faces(m_mesh)) {
		std::size_t face_indx = fccmap[f];
		
		Point_3 old_barycenter = barycenters[face_indx];

		// Get the vertices of face f
		auto h = m_mesh.halfedge(f);
		Point_3 v1 = m_mesh.point(m_mesh.target(h));
		Point_3 v2 = m_mesh.point(m_mesh.target(m_mesh.next(h)));
		Point_3 v3 = m_mesh.point(m_mesh.target(m_mesh.next(m_mesh.next(h))));

		double w_x = old_barycenter[0] + (v1[0] + v2[0] + v3[0]) / (3 * nb_per_cc[face_indx]);
		double w_y = old_barycenter[1] + (v1[1] + v2[1] + v3[1]) / (3 * nb_per_cc[face_indx]);
		double w_z = old_barycenter[2] + (v1[2] + v2[2] + v3[2]) / (3 * nb_per_cc[face_indx]);

		barycenters[face_indx] = Point_3(w_x, w_y, w_z);
	}

	for (face_descriptor f : faces(m_mesh)) {
		std::size_t face_indx = fccmap[f];

		Vector_3 barycenter = Vector_3(barycenters[face_indx][0], barycenters[face_indx][1], barycenters[face_indx][2]);

		//std::cout << "barycenter: " << barycenter[0] << " " << barycenter[1] << " " << barycenter[2] << std::endl;

		// Get the vertices of face f
		auto h = m_mesh.halfedge(f);
		auto v1 = m_mesh.point(m_mesh.target(h)) - barycenter;
		auto v2 = m_mesh.point(m_mesh.target(m_mesh.next(h))) - barycenter;
		auto v3 = m_mesh.point(m_mesh.target(m_mesh.next(m_mesh.next(h)))) - barycenter;

		// Compute volume of the tetrahedron
		Vector_3 cross_prod = CGAL::cross_product(v2 - v1, v3 - v1);
		//std::cout << "cross_prod: " << cross_prod[0] << " " << cross_prod[1] << " " << cross_prod[2] << std::endl;

		double volume = cross_prod[0] * v1[0] + cross_prod[1] * v1[1] + cross_prod[2] * v1[2];
		volumes[face_indx] += volume;
		//std::cout << volume << std::endl;
	}

	for (auto& [key, value] : volumes) {
		V += std::abs(value);
	}	

	m_volume2 = V / 6.0;
}

void FeatureExtraction::get_bounding_box_statistics() {
	// Compute the bounding box with CGAL
	Tree tree(faces(m_mesh).first, faces(m_mesh).second, m_mesh);

	// Compute the bounding box
	CGAL::Bbox_3 bbox = tree.bbox();

	// Get the diagonal length of the bounding box
	K::FT dx = bbox.xmax() - bbox.xmin();
	K::FT dy = bbox.ymax() - bbox.ymin();
	K::FT dz = bbox.zmax() - bbox.zmin();

	m_bounding_box_diameter = std::sqrt(dx * dx + dy * dy + dz * dz);
	m_bounding_box_volume = dx * dy * dz;
}

void FeatureExtraction::get_diameter() {
	// Compute the diameter of the mesh by finding the two points that are the farthest apart in the mesh

	// Loop over all the vertices in the mesh
	// For each vertex, loop over all the other vertices in the mesh
	// Compute the distance between the two vertices
	// If the distance is greater than the current maximum distance, update the maximum distance
	// Return the maximum distance as the diameter of the mesh

	// NOTE: for the exact diamter, a double for loop is required, but good approximations exist using the halfedge datastructure time in linear (I think) time. 
	// NOTE: use the convex hull, since diameter will be the same.

	double max_distance = 0.0;
	for (auto v : m_convexhull.vertices()) {
		for (auto u : m_convexhull.vertices()) {
			double distance = CGAL::squared_distance(m_convexhull.point(v), m_convexhull.point(u));
			if (distance > max_distance) {
				max_distance = distance;
			}
		}
	}

	// Set the diameter of the mesh
	m_diameter = std::sqrt(max_distance);
}

void FeatureExtraction::get_convexity() {
	// Compute the convex hull of the mesh with CGAL
	//Polyhedron_3 convexHull;
	//CGAL::convex_hull_3(m_mesh.points().begin(), m_mesh.points().end(), convexHull);

	// Compute the volume of the convex hull
	//m_hull_volume = CGAL::Polygon_mesh_processing::volume(convexHull);
	//m_hull_volume = CGAL::Polygon_mesh_processing::volume(m_convexhull);

	// Compute the surface area of the convex hull (maybe useful?)
	// m_hull_surface_area = CGAL::Polygon_mesh_processing::area(convexHull); 

	// Compute the convexity
	m_convexity = m_volume / m_hull_volume;

}

void FeatureExtraction::get_eccentricities() {
	/*
		Usually, eccentricity is defined as c/a where c is the distance between center and focus points and a is distance between focus and directrix.
		This comes down to e=sqrt(1-b^2/a^2) where b is the minor axis and a is the major axis.

		For the eigenvalue decomposition, we can treat the eigenvalues as the lengths of the axes of an ellipsoid.
		Then, the eccentricity is defined as using the ratio of the largest eigenvalue to the smallest eigenvalue.

		So, 1 will be very elliptical and 0 will be very spherical.
	*/


	// Compute the eigenvalue matrix using Eigen
	Eigen::Matrix3f covarianceMatrix = Eigen::Matrix3f::Zero();

	// Assumes that barycenter is at the origin

	for (const auto& vertex : m_mesh.vertices()) {
		Point_3 p = m_mesh.point(vertex);
		Eigen::Vector3f v(p.x(), p.y(), p.z());

		covarianceMatrix += v * v.transpose();
	}
	covarianceMatrix /= num_vertices(m_mesh);

	Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> eigenSolver(covarianceMatrix);
	Eigen::Vector3f eigenvalues = eigenSolver.eigenvalues();

	// eigenvalues(0) is the smallest and eigenvalues(2) is the largest

	// Compute the eccentricity (could also compute more eccentricities?
	
	//m_eccentricity = std::abs(eigenvalues(0) / eigenvalues(2)); // This formula does not make sense to me
	m_eccentricity02 = std::sqrt(1 - (eigenvalues(0) * eigenvalues(0)) / (eigenvalues(2) * eigenvalues(2)));
	m_eccentricity01 = std::sqrt(1 - (eigenvalues(0) * eigenvalues(0)) / (eigenvalues(1) * eigenvalues(1)));
	m_eccentricity12 = std::sqrt(1 - (eigenvalues(1) * eigenvalues(1)) / (eigenvalues(2) * eigenvalues(2)));


}

void FeatureExtraction::get_compactness() {
	// Compactness = (36 * pi * V^2) / (A^3)
	m_compactness =  (m_surface_area * m_surface_area * m_surface_area) / (36 * 3.1415926535897323847 * m_volume * m_volume);
}

void FeatureExtraction::get_sphericity() {
	m_sphericity = 1 / m_compactness;
}

void FeatureExtraction::get_rectangularity() {
	// Rectangularity = (V / (L1 * L2 * L3))
	// where L1, L2, L3 are the lengths of the sides of the bounding box
	m_rectangularity = m_volume / m_bounding_box_volume;
}


void FeatureExtraction::get_N_random_vertices(int N) {
	// Reserve space for the random vertices
	m_random_vertices.clear();
	m_random_vertices.reserve(N);

	// Get the area of all triangles of the mesh
	std::vector<double> triangle_areas;
	triangle_areas.reserve(num_faces(m_mesh));

	for (auto f : m_mesh.faces()) {
		auto h = m_mesh.halfedge(f);
		auto v1 = m_mesh.point(m_mesh.target(h));
		auto v2 = m_mesh.point(m_mesh.target(m_mesh.next(h)));
		auto v3 = m_mesh.point(m_mesh.target(m_mesh.next(m_mesh.next(h))));

		// Compute the area of the triangle by using the cross product
		double A = 0.5 * std::sqrt(CGAL::cross_product(v2 - v1, v3 - v1).squared_length());
		triangle_areas.push_back(A);
	}

	// Sample a random triangle based on the area of the triangle
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis_triangle(0, std::accumulate(triangle_areas.begin(), triangle_areas.end(), 0.0)); // For sampling a triangle
	std::uniform_real_distribution<> dis_point(0, 1); // For sampling coordinates within a triangle

	for (int i = 0; i < N; i++) {
		double random_area = dis_triangle(gen);

		// Find the triangle that corresponds to the random area
		double sum = 0.0;
		for (int j = 0; j < triangle_areas.size(); j++) {
			sum += triangle_areas[j];
			if (sum >= random_area) {
				auto f = m_mesh.faces_begin() + j;
				auto h = m_mesh.halfedge(*f);
				Point_3 v1 = m_mesh.point(m_mesh.target(h));
				Point_3 v2 = m_mesh.point(m_mesh.target(m_mesh.next(h)));
				Point_3 v3 = m_mesh.point(m_mesh.target(m_mesh.next(m_mesh.next(h))));

				// Compute three random numbers between 0 and 1 to sample random point on a triangle: https://stackoverflow.com/questions/4778147/sample-random-point-in-triangle
				double r1 = dis_point(gen);
				double r2 = dis_point(gen);

				double c1 = 1 - std::sqrt(r1);
				double c2 = std::sqrt(r1) * (1 - r2);
				double c3 = r2 * std::sqrt(r1);

				double w_x = c1 * v1[0] + c2 * v2[0] + c3 * v3[0];
				double w_y = c1 * v1[1] + c2 * v2[1] + c3 * v3[1];
				double w_z = c1 * v1[2] + c2 * v2[2] + c3 * v3[2];

				Point_3 random_point = Point_3(w_x, w_y, w_z);
				m_random_vertices.push_back(random_point);
				break;
			}
		}
	}
}

void FeatureExtraction::load_samplepoints(std::string& filename) {
	m_random_vertices.clear();
	std::cout << "Loading sample points from " << filename << std::endl;
	std::ifstream file(filename);
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		double x, y, z;
		if (!(iss >> x >> y >> z)) {
			break;
		}
		m_random_vertices.push_back(Point_3(x, y, z));
	}
	//std::cout << "Number of vertices: " << m_random_vertices.size() << std::endl;
	//std::cout << "First vertex: " << m_random_vertices[0][0] << "," << m_random_vertices[0][1] << "," << m_random_vertices[0][2] << std::endl;
}


void FeatureExtraction::get_A3_histogram() {
	// Assume points are loaded into m_random_vertices

	unsigned int iterations = m_random_vertices.size() / 3;

	Point_3 p1;
	Point_3 p2;
	Point_3 p3;
	Vector_3 v1;
	Vector_3 v2;
	float dot_product = 0.0f;

	for (unsigned int i = 0; i < iterations; i++) {
		// Compute the angle between the points m_random_vertices[3*i], m_random_vertices[3*i+1], m_random_vertices[3*i+2]

		p1 = m_random_vertices[3 * i];
		p2 = m_random_vertices[3 * i + 1];
		p3 = m_random_vertices[3 * i + 2];

		Vector_3 v1 = p1 - p2;
		Vector_3 v2 = p3 - p2;
		v1 /= std::sqrt(v1.squared_length());
		v2 /= std::sqrt(v2.squared_length());
		dot_product = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]; // Could take arccos, but also could leave it like this...
		double angle = std::acos(dot_product);

		if (angle < 0.0f) angle = 0.0f;
		if (angle > RBOUND_A3) angle = RBOUND_A3;

		assert(angle >= 0.0f && angle <= RBOUND_A3);

		// Increment counter of bins at the correct position
		int bin = get_bin(angle, NUM_A3_BINS, 0, RBOUND_A3); // Assume angle is between 0 and pi radians
		
		A3_bins[bin] += 1.0f / iterations;
	}
}

void FeatureExtraction::get_D1_histogram() {
	// Assume points are loaded into m_random_vertices

	unsigned int iterations = m_random_vertices.size();

	Point_3 p1;
	float distance = 0.0f;

	for (unsigned int i = 0; i < iterations; i++) {
		p1 = m_random_vertices[i];
		distance = std::sqrt(p1[0] * p1[0] + p1[1] * p1[1] + p1[2] * p1[2]);

		assert(distance >= 0.0f && distance <= RBOUND_D1);

		// Increment counter of bins at the correct position
		int bin = get_bin(distance, NUM_D1_BINS, 0, RBOUND_D1); // Assume angle is between 0 and pi radians
		D1_bins[bin] += 1.0f / iterations;
	}
}

void FeatureExtraction::get_D2_histogram() {
	// Assume points are loaded into m_random_vertices

	unsigned int iterations = m_random_vertices.size()/2;

	Point_3 p1;
	Point_3 p2;
	float distance = 0.0f;

	for (unsigned int i = 0; i < iterations; i++) {
		p1 = m_random_vertices[2*i];
		p2 = m_random_vertices[2 * i + 1];
		distance = std::sqrt((p1[0] - p2[0]) * (p1[0] - p2[0]) + (p1[1] - p2[1]) * (p1[1] - p2[1]) + (p1[2] - p2[2]) * (p1[2] - p2[2]));

		assert(distance >= 0.0f && distance <= RBOUND_D2);

		// Increment counter of bins at the correct position
		int bin = get_bin(distance, NUM_D2_BINS, 0, RBOUND_D2); // Assume angle is between 0 and pi radians
		D2_bins[bin] += 1.0f / iterations;
	}
}

void FeatureExtraction::get_D3_histogram() {
	// Assume points are loaded into m_random_vertices

	unsigned int iterations = m_random_vertices.size() / 3;

	Point_3 p1;
	Point_3 p2;
	Point_3 p3;
	Vector_3 v1;
	Vector_3 v2;
	float area = 0.0f;

	for (unsigned int i = 0; i < iterations; i++) {
		p1 = m_random_vertices[3 * i];
		p2 = m_random_vertices[3 * i + 1];
		p3 = m_random_vertices[3 * i + 2];
		
		// Compute area of triangle defined bu p1, p2 and p3
		v1 = p1 - p2;
		v2 = p3 - p2;

		Vector_3 cross_prod = CGAL::cross_product(v1, v2);
		area = std::sqrt(cross_prod.squared_length());

		assert(area >= 0.0f && std::sqrt(area) <= RBOUND_D3);

		// Increment counter of bins at the correct position
		int bin = get_bin(std::sqrt(area), NUM_D3_BINS, 0, RBOUND_D3); // Assume angle is between 0 and pi radians
		D3_bins[bin] += 1.0f / iterations;
	}
}

void FeatureExtraction::get_D4_histogram() {
	// Assume points are loaded into m_random_vertices

	unsigned int iterations = m_random_vertices.size() / 4;

	Point_3 p1;
	Point_3 p2;
	Point_3 p3;
	Point_3 p4;
	Vector_3 v1;
	Vector_3 v2;
	Vector_3 v3;
	float volume = 0.0f;

	for (unsigned int i = 0; i < iterations; i++) {
		p1 = m_random_vertices[4 * i];
		p2 = m_random_vertices[4 * i + 1];
		p3 = m_random_vertices[4 * i + 2];
		p4 = m_random_vertices[4 * i + 3];

		// Compute volume of tetrahedron defined by p1, p2, p3 and p4
		v1 = p1 - p2;
		v2 = p3 - p2;
		v3 = p4 - p2;

		Vector_3 cross_prod = CGAL::cross_product(v2, v3);
		volume = std::abs(cross_prod * v1) / 6.0;

		assert(volume >= 0.0f && std::cbrt(volume) <= RBOUND_D4);

		// Increment counter of bins at the correct position
		int bin = get_bin(std::cbrt(volume), NUM_D4_BINS, 0, RBOUND_D4); // Assume angle is between 0 and pi radians
		D4_bins[bin] += 1.0f / iterations;
	}
}


int FeatureExtraction::get_bin(double val, int N, double lboud, double rbound) {
	// returns the integer bin 0, ..., N-1 that val falls into given the domain of the variable

	// [lboud, lboud + (rbound - lbound) / N) = 0
	// [lboud + (rbound - lbound) / N, lboud + 2 * (rbound - lbound) / N) = 1
	// ...

	int bin = (int)std::floor((val - lboud) * N / (rbound - lboud));
	
	// Clamp
	if (bin < 0) {
		bin = 0;
	}
	else if (bin >= N) {
		bin = N - 1;
	}

	return bin;
}
