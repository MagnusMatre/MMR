#include "CGALclean.h"

CGALclean::CGALclean(std::string& input_dir, std::string& output_dir, int target, int threshold) : m_input_dir(input_dir), m_output_dir(output_dir), m_target(target), m_threshold(threshold) {}

void CGALclean::Execute() {

}

//void CGALclean::repair_and_remesh(std::vector<Point_3>& points, std::vector<Polygon_3>& faces, Mesh& mesh, bool& is_success) {
//	// Repairing.. 
//	//double diag = getAABoundingBoxDiag(points);
//
//	//polygon_soup_repair(points, faces);
//
//	// Check if the polygon soup describes a mesh
//	if (!CGALPMP::is_polygon_soup_a_polygon_mesh(faces)) {
//		std::cerr << "Error: the polygon soup does not describe a mesh. Skipping this one..." << std::endl;
//		is_success = false;
//		return;
//	}
//
//	//CGALPMP::polygon_soup_to_polygon_mesh(points, faces, mesh);
//
//	CGALPMP::orient(mesh);
//	CGALPMP::stitch_borders(mesh);
//
//	// Remeshing loop...
//	int remesh_iterations = 0;
//
//	double targetEdgePercentage = 0.025;
//	unsigned int nb_iter = 3;
//	int vertex_num = num_vertices(mesh);
//
//	int prev_vertex_num = vertex_num;
//
//	Mesh original_mesh = mesh;
//
//	// If it is more than 10000 vertices over, then double the targetEdgePercentage
//	// If it is more than 3000 vetices under, half the targetEdgePercentage
//	// 
//	// Otherwise, increase targetEdgePercentage by 2 * (1 - (10000 - vertex_num)/10000)
//	
//
//	while (remesh_iterations < m_max_remesh_iterations) {
//
//		double target_edge_length = diag * targetEdgePercentage;
//
//		std::vector<edge_descriptor> border;
//		CGALPMP::border_halfedges(CGAL::faces(mesh), mesh, boost::make_function_output_iterator(halfedge2edge(mesh, border)));
//		CGALPMP::split_long_edges(border, target_edge_length, mesh);
//
//		CGALPMP::isotropic_remeshing(CGAL::faces(mesh), target_edge_length, mesh,
//			CGAL::parameters::number_of_iterations(nb_iter)
//			.protect_constraints(true)); //i.e. protect border, h
//
//		prev_vertex_num = vertex_num;
//		vertex_num = num_vertices(mesh);
//		std::cout << "Number of vertices: " << vertex_num << " --- targetEdgePercentage: " << targetEdgePercentage << std::endl;
//
//		if (vertex_num == prev_vertex_num) {
//			std::cout << "Remeshing is not doing anything for this mesh..." << std::endl;
//			is_success = false;
//			break;
//		}
//		else if (vertex_num > m_target + m_threshold) {
//			std::cout << "Too many vertices... increasing targetEdgePercentage" << std::endl;
//			targetEdgePercentage = targetEdgePercentage * ( 1.9/(10000-m_target) * vertex_num + 3 - 10000 * 0.9/(10000 - m_target)); // Linear interpolation between 1.1 and 3 from m_target to 10000
//			mesh = original_mesh;
//		}
//		else if (vertex_num < m_target - m_threshold) {
//			std::cout << "Too few vertices... decreasing targetEdgePercentage" << std::endl;
//			targetEdgePercentage = targetEdgePercentage * (0.4 / m_target * vertex_num + 0.5); // Linear interpolation between 0.5 and 0.9 from 0 to m_target
//			mesh = original_mesh;
//		}
//		else {
//			std::cout << "Remeshing is done" << std::endl;
//			break;
//		}
//
//		remesh_iterations++;
//	}
//
//	if (remesh_iterations == m_max_remesh_iterations) {
//		std::cout << "Max remesh iterations reached, saving mesh with " << vertex_num << " vertices" << std::endl;
//		mesh = original_mesh;
//		is_success = false;
//	}
//
//	std::cout << "Done with remeshing: " << vertex_num <<  std::endl;
//}

void CGALclean::remesh(Mesh& mesh) {
	// Perform isotropic remeshing

	std::cout << "Start remeshing..." << std::endl;

	double diag = getAABoundingBoxDiag(mesh);

	double target_edge_length = 0.02 * diag; // Create fine cells, with hopefully more than 4000 vertices
	int nb_iter = 3;

	std::vector<edge_descriptor> border;
	CGALPMP::border_halfedges(CGAL::faces(mesh), mesh, boost::make_function_output_iterator(halfedge2edge(mesh, border)));
	CGALPMP::split_long_edges(border, target_edge_length, mesh);

	CGALPMP::isotropic_remeshing(CGAL::faces(mesh), target_edge_length, mesh,
		CGAL::parameters::number_of_iterations(nb_iter)
		.protect_constraints(true)); //i.e. protect border, h

	std::cout << "After remeshing mesh has " << num_vertices(mesh) << " vertices" << std::endl;

	//while (num_vertices(mesh) > m_target + m_threshold) {
	//	// Keep reducing the number of edges by half until the target is reached (make this a dynamic parameter)
	//	std::cout << "Start edge collapse loop at " << num_vertices(mesh) << " vertices" << std::endl;

	//	CGAL::Surface_mesh_simplification::Edge_count_ratio_stop_predicate<Mesh> stop(0.5);

	//	CGAL::Surface_mesh_simplification::edge_collapse(mesh, stop);
	//}

	//std::cout << "Finished with " << num_vertices(mesh) << " vertices" << std::endl;


}

double CGALclean::getAABoundingBoxDiag(Mesh& mesh) {
	Tree tree(faces(mesh).first, faces(mesh).second, mesh);

	// Compute the bounding box
	CGAL::Bbox_3 bbox = tree.bbox();

	// Get the diagonal length of the bounding box
	K::FT dx = bbox.xmax() - bbox.xmin();
	K::FT dy = bbox.ymax() - bbox.ymin();
	K::FT dz = bbox.zmax() - bbox.zmin();

	return std::sqrt(dx * dx + dy * dy + dz * dz);
}

//double CGALclean::getAABoundingBoxDiag(std::vector<Point_3> points) {
//	K::Iso_cuboid_3 c3 = CGAL::bounding_box(points.begin(), points.end());
//	double xdif = c3.xmax() - c3.xmin();
//	double ydif = c3.ymax() - c3.ymin();
//	double zdif = c3.zmax() - c3.zmin();
//
//	return CGAL::sqrt(xdif * xdif + ydif * ydif + zdif * zdif);
//}

//void CGALclean::fix_manifoldedness(Mesh& mesh) {
//
//	// Count non manifold vertices
//	int counter = 0;
//	for (vertex_descriptor v : vertices(mesh))
//	{
//		if (CGALPMP::is_non_manifold_vertex(v, mesh))
//		{
//			std::cout << "vertex " << v << " is non-manifold" << std::endl;
//			++counter;
//		}
//	}
//	std::cout << counter << " non-manifold occurrence(s)" << std::endl;
//
//	// Fix manifoldness by splitting non-manifold vertices
//	std::vector<std::vector<vertex_descriptor>> duplicated_vertices;
//	std::size_t new_vertices_nb = CGALPMP::duplicate_non_manifold_vertices(
//		mesh,
//		NP::output_iterator(std::back_inserter(duplicated_vertices))
//	);
//
//	std::cout << new_vertices_nb << " vertices have been added to fix mesh manifoldness" << std::endl;
//
//	for (std::size_t i = 0; i < duplicated_vertices.size(); ++i)
//	{
//		std::cout << "Non-manifold vertex " << duplicated_vertices[i].front() << " was fixed by creating";
//		for (std::size_t j = 1; j < duplicated_vertices[i].size(); ++j) {
//			std::cout << " " << duplicated_vertices[i][j];
//		}
//		std::cout << std::endl;
//	}
//
//}

//void CGALclean::polygon_soup_repair(std::vector<Point_3>& points, std::vector<Polygon_3>& faces) {
//	CGALPMP::orient_polygon_soup(points, faces);
//	CGALPMP::repair_polygon_soup(points, faces);
//
//	if (!CGALPMP::is_polygon_soup_a_polygon_mesh(faces)) {
//		CGALPMP::merge_duplicate_points_in_polygon_soup(points, faces);
//		CGALPMP::merge_duplicate_polygons_in_polygon_soup(points, faces);
//	}
//
//	if (!CGALPMP::is_polygon_soup_a_polygon_mesh(faces)) {
//		CGALPMP::duplicate_non_manifold_edges_in_polygon_soup(points, faces);
//	}
//
//	CGALPMP::remove_isolated_points_in_polygon_soup(points, faces);
//	CGALPMP::orient_polygon_soup(points, faces);
//}


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

void CGALclean::save_mesh(std::string& output_filename, ExactMesh& mesh) {
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

void CGALclean::save_mesh(std::string& output_filename, ExactPolyhedron& mesh) {
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
		return;
	}

	if (is_empty(mesh))
	{
		std::cerr << "Warning: empty file?" << std::endl;
		return;
	}

	std::cout << "Successfully loaded " << filename << std::endl;
}

void CGALclean::load_mesh(std::string& filename, ExactPolyhedron& mesh) {
	if (!CGALPMP::IO::read_polygon_mesh(filename, mesh)) {
		std::cerr << "Error: cannot read file " << filename << std::endl;
		return;
	}

	if (is_empty(mesh))
	{
		std::cerr << "Warning: empty file?" << std::endl;
		return;
	}

	std::cout << "Successfully loaded " << filename << std::endl;
}

void CGALclean::load_mesh(std::string& filename, ExactMesh& mesh) {
	if (!CGALPMP::IO::read_polygon_mesh(filename, mesh)) {
		std::cerr << "Error: cannot read file " << filename << std::endl;
		return;
	}

	if (is_empty(mesh))
	{
		std::cerr << "Warning: empty file?" << std::endl;
		return;
	}

	std::cout << "Successfully loaded " << filename << std::endl;
}


//void CGALclean::load_soup(std::string& filename, std::vector<Point_3>& points, std::vector<Polygon_3>& faces) {
//	std::ifstream input(filename);
//	if (!input) {
//		std::cerr << "Error: cannot open file " << filename << std::endl;
//		exit(EXIT_FAILURE);
//	}
//
//	if (!CGAL::IO::read_OBJ(input, points, faces)) {
//		std::cerr << "Error: cannot read file " << filename << std::endl;
//		exit(EXIT_FAILURE);
//	}
//	//CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, faces, mesh);
//
//}

void CGALclean::simple_subdivide(ExactMesh& mesh) {
	while (num_vertices(mesh) < m_target) {
		std::cout << "Doing subdivision loop..." << std::endl;
		CGAL::Subdivision_method_3::Loop_subdivision(mesh, CGAL::parameters::number_of_iterations(1));
	}
}

bool CGALclean::is_small_hole(exact_halfedge_descriptor h, ExactMesh& mesh, double max_hole_diam, int max_num_hole_edges){
	// Apapted from: https://github.com/oleg-alexandrov/cgal_tools/blob/master/fill_holes.
	int num_hole_edges = 0;
	CGAL::Bbox_3 hole_bbox;
	for (halfedge_descriptor hc : CGAL::halfedges_around_face(h, mesh))
	{
		const ExactPoint_3& p = mesh.point(target(hc, mesh));

		hole_bbox += p.bbox();
		++num_hole_edges;

		// Exit early, to avoid unnecessary traversal of large holes
		if (num_hole_edges > max_num_hole_edges) return false;
		if (hole_bbox.xmax() - hole_bbox.xmin() > max_hole_diam) return false;
		if (hole_bbox.ymax() - hole_bbox.ymin() > max_hole_diam) return false;
		if (hole_bbox.zmax() - hole_bbox.zmin() > max_hole_diam) return false;
	}

	return true;
}

void CGALclean::fill_holes(ExactMesh& mesh, double diag) {
	// Adapted from: https://github.com/oleg-alexandrov/cgal_tools/blob/master/fill_holes.

	CGALPMP::remove_degenerate_faces(mesh);
	CGALPMP::remove_degenerate_edges(mesh);
	CGALPMP::remove_isolated_vertices(mesh);

	if (CGAL::is_closed(mesh)) {
		std::cout << "Mesh is already closed, skipping hole filling" << std::endl;
		return;
	}

	double max_hole_diam = diag * 0.2; // Hole can be at most 20 percent of mesh bounding box diameter
	int max_num_hole_edges = 1000; // hole can only consist of at most 1000 edges

	unsigned int nb_holes = 0;
	std::vector<exact_halfedge_descriptor> border_cycles;

	CGALPMP::extract_boundary_cycles(mesh, std::back_inserter(border_cycles));

	for (exact_halfedge_descriptor h : border_cycles)
	{
		if (nb_holes > 100) { // fix at most 100 holes
			return;
		}
		if (max_hole_diam > 0 && max_num_hole_edges > 0 &&
			!is_small_hole(h, mesh, max_hole_diam, max_num_hole_edges))
			continue;

		std::vector<exact_face_descriptor>  patch_facets;
		std::vector<exact_vertex_descriptor> patch_vertices;
		CGALPMP::triangulate_and_refine_hole(mesh, h);

		//std::cout << "* Number of facets in constructed patch: " << patch_facets.size() << std::endl;
		//std::cout << "  Number of vertices in constructed patch: " << patch_vertices.size() << std::endl;
		//std::cout << "  Is fairing successful: " << success << std::endl;
		++nb_holes;
	}

}

void CGALclean::orient_mesh(ExactMesh& mesh) {
	// TODO: use CGAL's function to orient the mesh with respect to the volume it is bounding, if it is bounding a volume
	
	CGALPMP::remove_degenerate_faces(mesh);

	//bool intersecting = CGALPMP::does_self_intersect<CGAL::Parallel_if_available_tag>(mesh, CGAL::parameters::vertex_point_map(get(CGAL::vertex_point, mesh)));
	//if (intersecting) {
	//	std::vector<std::pair<face_descriptor, face_descriptor> > intersected_tris;
	//	CGALPMP::self_intersections<CGAL::Parallel_if_available_tag>(faces(mesh), mesh, std::back_inserter(intersected_tris));
	//	std::cout << intersected_tris.size() << " pairs of triangles intersect..." << std::endl;
	//	std::cout << "Removing them with experimental function..." << std::endl;
	//	CGALPMP::experimental::remove_self_intersections(mesh);
	//	//std::cout << intersected_tris.size() << " pairs of triangles intersect, not doing reorientation..." << std::endl;
	//	//return;
	//}

	if (CGAL::is_closed(mesh)) {
		CGALPMP::orient_to_bound_a_volume(mesh); // problem with colinearity...
	}
}

void CGALclean::refine(ExactPolyhedron& mesh) {

	std::vector<ExactPolyhedron::Facet_handle>  new_facets;
	std::vector<Vertex_handle> new_vertices;

	unsigned int n_vertices = mesh.size_of_vertices();

	while (n_vertices < m_target) {
		std::cout << "refine loop at " << n_vertices << " vertices" << std::endl;
		CGALPMP::refine(mesh, CGAL::faces(mesh), // Does not increase number of vertices
			std::back_inserter(new_facets),
			std::back_inserter(new_vertices),
			CGAL::parameters::density_control_factor(3) // default is sqrt 2
		);
		n_vertices = mesh.size_of_vertices();
	}
}

void CGALclean::smooth(ExactMesh& mesh) {
	std::set<ExactMesh::Vertex_index> constrained_vertices;
	for (ExactMesh::Vertex_index v : vertices(mesh))
	{
		if (is_border(v, mesh))
			constrained_vertices.insert(v);
	}

	std::cout << "Constraining: " << constrained_vertices.size() << " border vertices" << std::endl;
	CGAL::Boolean_property_map<std::set<ExactMesh::Vertex_index> > vcmap(constrained_vertices);

	unsigned int nb_iterations = 1;
	double time = 0.0001;

	std::cout << "Smoothing shape... (" << nb_iterations << " iterations)" << std::endl;
	CGALPMP::smooth_shape(mesh, time, CGAL::parameters::number_of_iterations(nb_iterations).vertex_is_constrained_map(vcmap));

}

void CGALclean::handle_all_directories(std::string& start_directory) {
	// Iterate over all .obj files in the directory and remesh the meshes inside it until it has the target number of vertices

	// Variables for mesh loading
	Mesh mesh;
	ExactMesh mesh_exact;
	ExactPolyhedron poly_exact;
	std::vector<Point_3> points;
	std::vector<Polygon_3> faces;

	int class_counter = 0;

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

		/*std::ofstream logFile;
		logFile.open(m_output_dir + "/" + directoryName + "/remeshLog.txt");*/

		std::cout << "Handling class folder: " << class_folder << std::endl;

		class_counter = 0;
		for (const auto& entry : std::filesystem::directory_iterator(m_input_dir + "/" + directoryName)) {
			//class_counter++;
			/*if (class_counter >= 10) {
				break;
			}*/
			mesh.clear();
			mesh_exact.clear();
			poly_exact.clear();
			points.clear();
			faces.clear();

			std::filesystem::path filePath(entry);
			std::string fileName = filePath.filename().string();
			std::string outputFile = m_output_dir + "/" + directoryName + "/" + fileName;

			// Check if the outputFile exists or if the file is in "skip_meshes", if it does, skip the current mesh
			//if (std::filesystem::exists(outputFile)) {
			//	std::cout << "File: " << outputFile << " already exists, skipping..." << std::endl;
			//	//logFile << "File: " << outputFile << " already exists, skipping..." << std::endl;
			//	continue;
			//}

			//Skip the file if it does not end in 

			bool is_success = true;

			// Load the mesh
			std::string input_name = entry.path().string();
			//load_soup(input_name, points, faces);
			//repair_and_remesh(points, faces, mesh, is_success);

			load_mesh(input_name, mesh_exact); // Not very efficient to load in mesh twice...
			//load_mesh(input_name, mesh);

			CGAL::Polygon_mesh_processing::orient_to_bound_a_volume(mesh_exact); // do this for convex hulls

			//double diag = getAABoundingBoxDiag(mesh);
				
			//fill_holes(mesh_exact, diag); // Fills all holes in the mesh to make it closed

			//simple_subdivide(mesh_exact);

			//refine(poly_exact);

			//smooth(mesh_exact);

			//load_mesh(input_name, mesh_exact);
			//remesh(mesh);

			//load_mesh(input_name, mesh_exact);

			//orient_mesh(mesh_exact); // Orients the mesh such that volumes point outwards (maybe this cannot be done with exact_halfedges?)
			 
			if (!is_success) {
				//logFile << input_name << " FAILED" << std::endl;
			}
			else {
				std::cout << "Saving mesh to file: " << " " << directoryName << "/" << fileName << " with resulting number of vertices: " << num_vertices(mesh_exact) << std::endl;
				//logFile << input_name << " SUCCESS, vertex count: " << num_vertices(mesh_exact) << std::endl;

				save_mesh(outputFile, mesh_exact);
			}

		}

		//logFile.close();
	}
}

