#include "FullRemesh.h"


FullRemesh::FullRemesh(std::string& mesh_directory, std::string& output_directory, int target) {
	m_mesh_directory = mesh_directory;
	m_output_dir = output_directory;
	m_target = target;

}


FullRemesh::~FullRemesh() {
	
}

void FullRemesh::handle_mesh(std::string& file_name, ExactMesh& CGALmesh) {

	bool loadSuccess = load_CGAL_mesh(file_name, CGALmesh); // Load mesh with CGAL

	if (!loadSuccess) {
		std::cerr << "Error loading mesh: " << file_name << std::endl;
		return;
	}

	fill_holes(CGALmesh, 0.2);			// Fill the holes, also with CGAL
	std::cout << "Filled holes for " << file_name << std::endl;

	do_subdivision(CGALmesh);			// Do subdivision loops until mesh contains enough vertices
	std::cout << "Subdivided " << file_name << std::endl;

	// Save the CGAL mesh to the temporary file
	std::string temp_filename = m_output_dir + "/temp.obj";
	bool saveSuccess = save_CGAL_mesh(temp_filename, CGALmesh);

	if (!saveSuccess) {
		std::cerr << "Error saving mesh: " << file_name << std::endl;
		return;
	}

	// Load the mesh again with vcglib
	MyMesh VCGmesh;
	if (vcg::tri::io::Importer<MyMesh>::Open(VCGmesh, temp_filename.c_str()) != 0) {
		std::cerr << "Error reading file: " << temp_filename << std::endl;
		return;
	}
	std::cout << "Starting VCG decimation on " << file_name << " with " << VCGmesh.VN() << " vertices" << std::endl;

	do_decimation(VCGmesh);

	if (vcg::tri::io::Exporter<MyMesh>::Save(VCGmesh, temp_filename.c_str())) {
		std::cerr << "Error writing file: " << file_name << std::endl;
		return;
	}

	// Load the mesh again with CGAL
	CGALmesh.clear();
	load_CGAL_mesh(temp_filename, CGALmesh);

	// delete the temporary file
	std::remove(temp_filename.c_str());
}

void FullRemesh::handle_all_directories() {
	std::string class_name = "";
	std::string file_name = "";

	ExactMesh CGALmesh;

	for (const auto& class_folder : std::filesystem::directory_iterator(m_mesh_directory)) {
		//Check if the current folder is a directory
		if (!std::filesystem::is_directory(class_folder)) {
			continue;
		}

		//Open a log file names "remeshLog.txt" to write the output log to
		std::filesystem::path class_folder_path(class_folder);
		std::string directoryName = class_folder_path.filename().string();

		std::cout << "Handling class folder: " << class_folder << std::endl;

		for (const auto& entry : std::filesystem::directory_iterator(m_mesh_directory + "/" + directoryName)) {
			//class_counter++;
			/*if (class_counter >= 10) {
				break;
			*/

			CGALmesh.clear();

			std::filesystem::path filePath(entry);
			std::string input_file = filePath.string();
			std::string fileName = filePath.filename().string();
			std::string outputFile = m_output_dir + "/" + directoryName + "/" + fileName;

			// Check if outputFile already exists
			if (std::filesystem::exists(outputFile)) {
				std::cout << "Skipping " << outputFile << " as it already exists" << std::endl;
				continue;
			}
			

			handle_mesh(input_file, CGALmesh);


			save_CGAL_mesh(outputFile, CGALmesh);

		}
	}

}

bool FullRemesh::load_CGAL_mesh(std::string& filename, ExactMesh& mesh) {
	if (!CGALPMP::IO::read_polygon_mesh(filename, mesh)) {
		std::cerr << "Error: cannot read file " << filename << std::endl;
		return false;
	}

	if (is_empty(mesh))
	{
		std::cerr << "Warning: empty file?" << std::endl;
		return false;
	}

	std::cout << "Successfully loaded " << filename << std::endl;
	return true;
}

bool FullRemesh::save_CGAL_mesh(std::string& output_filename, ExactMesh& mesh) {
	std::ofstream output(output_filename);
	if (!output) {
		std::cerr << "Error: cannot open file " << output_filename << std::endl;
		return false;
	}

	if (CGAL::IO::write_polygon_mesh(output_filename, mesh, CGAL::parameters::stream_precision(10))) {
		std::cout << "Successfully saved mesh to " << output_filename << std::endl;
	}
	else {
		std::cerr << "Error: cannot write file " << output_filename << std::endl;
		return false;
	}

	return true;
}

bool FullRemesh::is_small_hole(exact_halfedge_descriptor h, ExactMesh& mesh, double max_hole_diam, int max_num_hole_edges) {
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

void FullRemesh::fill_holes(ExactMesh& mesh, double diag) {
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

void FullRemesh::do_subdivision(ExactMesh& mesh) {
	while (num_vertices(mesh) < m_target) {
		std::cout << "Doing subdivision loop at " << num_vertices(mesh) << " vertices..." << std::endl;
		CGAL::Subdivision_method_3::Loop_subdivision(mesh, CGAL::parameters::number_of_iterations(1));
	}
}

void FullRemesh::do_decimation(MyMesh& mesh) {

	TriEdgeCollapseQuadricParameter qparams;
	qparams.QualityThr = .3;
	double TargetError = std::numeric_limits<double >::max();

	unsigned int cur_num_faces = mesh.FN();
	unsigned int cur_num_vertices = mesh.VN();

	unsigned int prev_num_vertices = 0;

	vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh);
	vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(mesh);

	vcg::tri::UpdateBounding<MyMesh>::Box(mesh);

	// Perform decimation using the Do method of EdgeCollapser

	vcg::LocalOptimization<MyMesh> DeciSession(mesh, &qparams);
	DeciSession.Init<MyTriEdgeCollapse>();

	unsigned int equal_cnt = 0;
	while (cur_num_vertices > m_target + 500) {
		std::cout << "Doing decimate loop... " << cur_num_faces << " faces " << cur_num_vertices << " vertices" << std::endl;
		DeciSession.SetTargetSimplices(0.95 * cur_num_faces);
		DeciSession.SetTimeBudget(0.5f);
		DeciSession.SetTargetOperations(100000);
		if (TargetError < std::numeric_limits<float>::max()) {
			DeciSession.SetTargetMetric(TargetError);
		}

		DeciSession.DoOptimization();

		prev_num_vertices = cur_num_vertices;
		cur_num_faces = mesh.FN();
		cur_num_vertices = mesh.VN();

		if (cur_num_vertices == prev_num_vertices) {
			equal_cnt++;
		}

		if (equal_cnt > 5) {
			break;
		}

	}

	// Output the result
	std::cout << "Final number of vertices: " << mesh.VN() << std::endl;
}