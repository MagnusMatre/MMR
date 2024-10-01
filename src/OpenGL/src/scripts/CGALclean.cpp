#include "CGALclean.h"

CGALclean::CGALclean(std::string& input_dir, std::string& output_dir, int target, int threshold) : m_input_dir(input_dir), m_output_dir(output_dir), m_target(target), m_threshold(threshold) {}

void CGALclean::Execute() {

}

void CGALclean::repair_and_remesh(std::vector<Point_3>& points, std::vector<Polygon_3>& faces, Mesh& mesh) {
	// Repairing.. 
	double diag = getAABoundingBoxDiag(points);

	polygon_soup_repair(points, faces);
	CGALPMP::polygon_soup_to_polygon_mesh(points, faces, mesh);

	CGALPMP::orient(mesh);
	CGALPMP::stitch_borders(mesh);

	// Remeshing loop...
	int remesh_iterations = 0;

	double targetEdgePercentage = 0.03;
	unsigned int nb_iter = 3;
	int vertex_num = num_vertices(mesh);

	int prev_vertex_num = vertex_num;

	Mesh original_mesh = mesh;

	// If it is more than 10000 vertices over, then double the targetEdgePercentage
	// If it is more than 3000 vetices under, half the targetEdgePercentage
	// 
	// Otherwise, increase targetEdgePercentage by 2 * (1 - (10000 - vertex_num)/10000)
	

	while (remesh_iterations < m_max_remesh_iterations) {

		double target_edge_length = diag * targetEdgePercentage;

		std::vector<edge_descriptor> border;
		CGALPMP::border_halfedges(CGAL::faces(mesh), mesh, boost::make_function_output_iterator(halfedge2edge(mesh, border)));
		CGALPMP::split_long_edges(border, target_edge_length, mesh);

		CGALPMP::isotropic_remeshing(CGAL::faces(mesh), target_edge_length, mesh,
			CGAL::parameters::number_of_iterations(nb_iter)
			.protect_constraints(true)); //i.e. protect border, h

		prev_vertex_num = vertex_num;
		vertex_num = num_vertices(mesh);
		std::cout << "Number of vertices: " << vertex_num << std::endl;

		if (vertex_num == prev_vertex_num) {
			std::cout << "Remeshing is not doing anything for this mesh..." << std::endl;
			break;
		}
		else if (vertex_num > m_target + m_threshold) {
			std::cout << "Too many vertices... increasing targetEdgePercentage" << std::endl;
			targetEdgePercentage = targetEdgePercentage * ( 0.9/(10000-m_target) * vertex_num + 2 - 10000 * 0.9/(10000 - m_target)); // Linear interpolation between 0.5 and 0.9
			mesh = original_mesh;
		}
		else if (vertex_num < m_target - m_threshold) {
			std::cout << "Too few vertices... decreasing targetEdgePercentage" << std::endl;
			targetEdgePercentage = targetEdgePercentage * (0.4 / m_target * vertex_num + 0.5); // Linear interpolation between 0.5 and 0.9
			mesh = original_mesh;
		}
		else {
			std::cout << "Remeshing is done" << std::endl;
			break;
		}

		remesh_iterations++;
	}

	if (remesh_iterations == m_max_remesh_iterations) {
		std::cout << "Max remesh iterations reached, saving mesh with " << vertex_num << " vertices" << std::endl;
		mesh = original_mesh;
	}

	std::cout << "Done with remeshing: " << vertex_num <<  std::endl;
}

double CGALclean::getAABoundingBoxDiag(std::vector<Point_3> points) {
	K::Iso_cuboid_3 c3 = CGAL::bounding_box(points.begin(), points.end());
	double xdif = c3.xmax() - c3.xmin();
	double ydif = c3.ymax() - c3.ymin();
	double zdif = c3.zmax() - c3.zmin();

	return CGAL::sqrt(xdif * xdif + ydif * ydif + zdif * zdif);
}

void CGALclean::fix_manifoldedness(Mesh& mesh) {

	// Count non manifold vertices
	int counter = 0;
	for (vertex_descriptor v : vertices(mesh))
	{
		if (CGALPMP::is_non_manifold_vertex(v, mesh))
		{
			std::cout << "vertex " << v << " is non-manifold" << std::endl;
			++counter;
		}
	}
	std::cout << counter << " non-manifold occurrence(s)" << std::endl;

	// Fix manifoldness by splitting non-manifold vertices
	std::vector<std::vector<vertex_descriptor>> duplicated_vertices;
	std::size_t new_vertices_nb = CGALPMP::duplicate_non_manifold_vertices(
		mesh,
		NP::output_iterator(std::back_inserter(duplicated_vertices))
	);

	std::cout << new_vertices_nb << " vertices have been added to fix mesh manifoldness" << std::endl;

	for (std::size_t i = 0; i < duplicated_vertices.size(); ++i)
	{
		std::cout << "Non-manifold vertex " << duplicated_vertices[i].front() << " was fixed by creating";
		for (std::size_t j = 1; j < duplicated_vertices[i].size(); ++j) {
			std::cout << " " << duplicated_vertices[i][j];
		}
		std::cout << std::endl;
	}

}

void CGALclean::polygon_soup_repair(std::vector<Point_3>& points, std::vector<Polygon_3>& faces) {
	CGALPMP::merge_duplicate_points_in_polygon_soup(points, faces);
	CGALPMP::merge_duplicate_polygons_in_polygon_soup(points, faces);
	CGALPMP::remove_isolated_points_in_polygon_soup(points, faces);
	CGALPMP::repair_polygon_soup(points, faces);
	CGALPMP::orient_polygon_soup(points, faces);

	CGALPMP::merge_duplicate_points_in_polygon_soup(points, faces);
	CGALPMP::merge_duplicate_polygons_in_polygon_soup(points, faces);
}


void CGALclean::save_mesh(std::string& output_filename, Mesh& mesh) {
	std::ofstream output(output_filename);
	if (!output) {
		std::cerr << "Error: cannot open file " << output_filename << std::endl;
		exit(EXIT_FAILURE);
	}

	if (CGAL::IO::write_polygon_mesh(output_filename, mesh, CGAL::parameters::stream_precision(17))) {
		std::cout << "Successfully saved mesh to " << output_filename << std::endl;
	}
	else {
		std::cerr << "Error: cannot write file " << output_filename << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CGALclean::load_mesh(std::string& filename, Mesh& mesh) {
	if (!CGALPMP::IO::read_polygon_mesh(filename, mesh)) {
		std::cerr << "Error: cannot read file " << filename << std::endl;
		exit(EXIT_FAILURE);
	}

	if (is_empty(mesh))
	{
		std::cerr << "Warning: empty file?" << std::endl;
		return;
	}

	std::cout << "Successfully loaded " << filename << std::endl;
}


void CGALclean::load_soup(std::string& filename, std::vector<Point_3>& points, std::vector<Polygon_3>& faces) {
	std::ifstream input(filename);
	if (!input) {
		std::cerr << "Error: cannot open file " << filename << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!CGAL::IO::read_OBJ(input, points, faces)) {
		std::cerr << "Error: cannot read file " << filename << std::endl;
		exit(EXIT_FAILURE);
	}
	//CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, faces, mesh);

}

void CGALclean::handle_all_directories(std::string& start_directory) {
	// Iterate over all .obj files in the directory and remesh the meshes inside it until it has the target number of vertices

	// Variables for mesh loading
	Mesh mesh;
	std::vector<Point_3> points;
	std::vector<Polygon_3> faces;

	for (const auto& class_folder : std::filesystem::directory_iterator(m_input_dir)) {
		//Check if the current folder is a directory
		if (!std::filesystem::is_directory(class_folder)) {
			continue;
		}

		//Open a log file names "remeshLog.txt" to write the output log to
		std::filesystem::path class_folder_path(class_folder);
		std::string directoryName = class_folder_path.filename().string();

		if (directoryName < start_directory) { // Skip all folders before the start_dir
			std::cout << "Skipping class folder: " << class_folder << std::endl;
			continue;
		}

		std::ofstream logFile;
		logFile.open(m_output_dir + "/" + directoryName + "/remeshLog.txt");

		std::cout << "Handling class folder: " << class_folder << std::endl;

		for (const auto& entry : std::filesystem::directory_iterator(m_input_dir + "/" + directoryName)) {
			mesh.clear();
			points.clear();
			faces.clear();

			std::filesystem::path filePath(entry);
			std::string fileName = filePath.filename().string();
			std::string outputFile = m_output_dir + "/" + directoryName + "/" + fileName;

			// Check if the outputFile exists or if the file is in "skip_meshes", if it does, skip the current mesh
			if (std::filesystem::exists(outputFile)) {
				std::cout << "File: " << outputFile << " already exists, skipping..." << std::endl;
				logFile << "File: " << outputFile << " already exists, skipping..." << std::endl;
				continue;
			}

			// Load the mesh
			std::string input_name = entry.path().string();
			load_soup(input_name, points, faces);
			repair_and_remesh(points, faces, mesh);

			std::cout << "Saving mesh to file: " << " " << directoryName << "/" << fileName << " with resulting number of vertices: " << num_vertices(mesh) << std::endl;
			logFile << "Saving mesh to file: " << " " << directoryName << "/" << fileName << " with resulting number of vertices: " << num_vertices(mesh)	<< std::endl;

			save_mesh(outputFile, mesh);
		}

		logFile.close();
	}
}

