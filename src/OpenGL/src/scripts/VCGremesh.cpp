#include "VCGremesh.h"

#include <iostream>
#include <filesystem>
#include <fstream>

#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>

#include<vcg/complex/algorithms/isotropic_remeshing.h>
#include <vcg/complex/algorithms/refine.h>

#include <vcg/math/perlin_noise.h>
#include<vcg/complex/algorithms/create/resampler.h>

#include<vcg/complex/algorithms/point_sampling.h>
#include<vcg/complex/algorithms/create/platonic.h>

#include <vcg/complex/algorithms/edge_collapse.h> // Edge collapse algorithm
#include "vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h"

VCGremesh::VCGremesh(std::string& input_dir, std::string& output_dir) {
	m_input_dir = input_dir;
	m_output_dir = output_dir;

}

VCGremesh::~VCGremesh() {
}

void VCGremesh::Execute() {
	// Code to run VCGremesh
	//std::cout << "Running VCGremesh" << std::endl;
	//remesh_directory();
}

void VCGremesh::resample_mesh(MyMesh& mesh, MyMesh& output_mesh) {
	vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh);
	vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(mesh);
	vcg::tri::Clean<MyMesh>::RemoveDegenerateFace(mesh);

	vcg::tri::Allocator<MyMesh>::CompactEveryVector(mesh);
	vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalizedPerFaceNormalized(mesh);

	vcg::tri::UpdateBounding<MyMesh>::Box(mesh);

	float alpha = 50; // initially start with alpha = 50
	int it_num = 0;

	vcg::Box3f bb = mesh.bbox;
	int cur_num_vertices = mesh.VN();

	while (cur_num_vertices < (m_target - m_threshold) || cur_num_vertices > (m_target + m_threshold)) { // While number of vertices is less, remesh with small cell side
		vcg::Box3f bb = mesh.bbox;
		float cell_side = bb.Diag() / alpha;
		bb.Offset(cell_side);
		vcg::Point3i box_size = vcg::Point3i(bb.DimX() / cell_side, bb.DimY() / cell_side, bb.DimZ() / cell_side);
		vcg::tri::Resampler<MyMesh, MyMesh>::Resample(mesh, output_mesh, bb, box_size, cell_side*10);

		cur_num_vertices = output_mesh.VN();

		if (cur_num_vertices < (m_target - m_threshold)) {
			alpha = alpha * 1.2;
			std::cout << it_num << ": Too few vertices ("<< cur_num_vertices<<")... increasing alpha to " << alpha << std::endl;
		}
		else if (cur_num_vertices > (m_target + m_threshold)) {
			alpha = alpha * 0.8;
			std::cout << it_num << ": Too many vertices(" << cur_num_vertices << ")... decreasing alpha to " << alpha << std::endl;
		}
		else if (it_num >= 5) {
			break;
		}
		else {
			break;
		}
		it_num++;
	}

	std::cout << "Resampling done with " << output_mesh.VN() << " vertices" << std::endl;
	
}

void VCGremesh::remesh_mesh(MyMesh& mesh, MyMesh& output_mesh) {
	// Mesh cleaning

	vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh);
	vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(mesh);
	vcg::tri::Clean<MyMesh>::RemoveDegenerateFace(mesh);

	vcg::tri::Allocator<MyMesh>::CompactEveryVector(mesh);

	vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalizedPerFaceNormalized(mesh);
	vcg::tri::UpdateBounding<MyMesh>::Box(mesh);

	vcg::tri::Append<MyMesh, MyMesh>::MeshCopy(output_mesh, mesh);
	vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalizedPerFaceNormalized(output_mesh);
	vcg::tri::UpdateBounding<MyMesh>::Box(output_mesh);

	vcg::tri::UpdateTopology<MyMesh>::FaceFace(output_mesh);
	//vcg::tri::UpdateTopology<MyMesh>::VertexFace(m_output_mesh);
	//vcg::tri::UpdateTopology<MyMesh>::VertexVertex(mesh);


	// Skip mesh if it already has fewer than target number of vertices
	if (mesh.VN() <= m_target + m_threshold) {
		printf("Skipping mesh with %i vertices\n", mesh.VN());
		return;
	}

	float targetLenPerc = 0.1f;
	float maxSurfDistPerc = 0.001f;
	int iterNum = 1;
	float creaseAngle = 30.f;
	//float maxSurfDistPerc = 0.001f;

	int num_of_vertices = output_mesh.VN();

	float lengthThr = targetLenPerc * (mesh.bbox.Diag() / 100.f);
	float maxSurfDist = maxSurfDistPerc * (mesh.bbox.Diag() / 100.f);
	//float maxSurfDist = maxSurfDistPerc * (mesh.bbox.Diag() / 100.f);
	printf("Length Thr: %8.3f ~ %4.2f %% on %5.3f\n", lengthThr, targetLenPerc, mesh.bbox.Diag());
	printf(" Input mesh %8i v %8i f\n", mesh.VN(), mesh.FN());

	while (num_of_vertices > m_target + m_threshold) {
		vcg::tri::IsotropicRemeshing<MyMesh>::Params params;
		params.SetTargetLen(targetLenPerc * (mesh.bbox.Diag() / 100.f));
		params.SetFeatureAngleDeg(creaseAngle);
		params.iter = iterNum;
		params.cleanFlag = true;
		params.userSelectedCreases = false;

		if (maxSurfDistPerc != 0)
		{
			params.surfDistCheck = true;
			params.maxSurfDist = maxSurfDistPerc * (output_mesh.bbox.Diag() / 100.f);
		}
		else
		{
			params.surfDistCheck = false;
		}
		vcg::tri::IsotropicRemeshing<MyMesh>::Do(output_mesh, mesh, params);

		vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(output_mesh);
		vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(output_mesh);
		vcg::tri::Clean<MyMesh>::RemoveDegenerateFace(output_mesh);

		vcg::tri::Allocator<MyMesh>::CompactEveryVector(output_mesh);

		printf("Output mesh %8i v %8i f\n", output_mesh.VN(), output_mesh.FN());

		num_of_vertices = output_mesh.VN();
		if (num_of_vertices < m_target - m_threshold) {
			targetLenPerc = targetLenPerc * 0.5;
			printf("Too few vertices... decreasing targetLenPerc to %f\n", targetLenPerc);
		}
		else if (num_of_vertices > m_target + m_threshold) {
			targetLenPerc = targetLenPerc * 2;
			printf("Too many vertices... increasing targetLenPerc to %f\n", targetLenPerc);
		}
		else {
			break;
		}
	}

	std::cout << "Remeshing done with " << output_mesh.VN() << " vertices" << std::endl;
}

//void VCGremesh::decimate_mesh(MyMesh& mesh, MyMesh& output_mesh) {
//	vcg::tri::TriEdgeCollapseQuadricParameter params;
//	params.QualityThr = 0.3;
//
//	vcg::LocalOptimization<MyMesh> DeciSession(mesh, params);
//	DeciSession.Init<vcg::LocalOptimization>(2500);
//	DeciSession.DoOptimization();
//}

void VCGremesh::edge_collapse(MyMesh& mesh, MyMesh& output_mesh) {
	

	vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh);
	vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(mesh);
	vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
	vcg::tri::UpdateNormal<MyMesh>::PerVertexAngleWeighted(mesh);
	vcg::tri::Allocator<MyMesh>::CompactEveryVector(mesh);
	vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);

	tri::UpdateBounding<MyMesh>::Box(mesh);
	tri::UpdateNormal<MyMesh>::PerVertexNormalized(mesh);
	tri::UpdateQuality<MyMesh>::VertexConstant(mesh, 1.0);

	

	// Decimate the mesh using EdgeCollapser
	vcg::tri::EdgeCollapser<MyMesh, vcg::tri::BasicVertexPair<MyVertex>> EdgeCollapser;

	// Perform decimation using the Do method of EdgeCollapser
	//EdgeCollapser.Do(mesh, output_mesh, 2500, false);



	// Output the result
	std::cout << "Final number of vertices: " << mesh.vert.size() << std::endl;
}

// Iterate over all .obj files in the directory and remesh the meshes inside it until it has the target number of vertices
void VCGremesh::remesh_all_directories(std::string& start_dir){
	for (const auto& class_folder : std::filesystem::directory_iterator(m_input_dir)) {
		//Check if the current folder is a directory
		if (!std::filesystem::is_directory(class_folder)) {
			continue;
		}

		//Open a log file names "remeshLog.txt" to write the output log to
		std::filesystem::path class_folder_path(class_folder);
		std::string directoryName = class_folder_path.filename().string();

		if (directoryName < start_dir) { // Skip all folders before the start_dir
			std::cout << "Skipping class folder: " << class_folder << std::endl;
			continue;
		}

		std::ofstream logFile;
		logFile.open(m_output_dir + "/" + directoryName + "/remeshLog.txt");

		std::cout << "Handling class folder: " << class_folder << std::endl;

		for (const auto& entry : std::filesystem::directory_iterator(m_input_dir + "/" + directoryName)) {
			std::filesystem::path filePath(entry);
			std::string fileName = filePath.filename().string();
			std::string outputFile = m_output_dir + "/" + directoryName + "/" + fileName;

			// Check if the outputFile exists or if the file is in "skip_meshes", if it does, skip the current mesh
			if (std::filesystem::exists(outputFile) || m_to_skip.find(directoryName + "/" + fileName) != m_to_skip.end()) {
				std::cout << "File: " << outputFile << " already exists, skipping..." << std::endl;
				logFile << "File: " << outputFile << " already exists, skipping..." << std::endl;
				continue;
			}

			// Load the mesh
			MyMesh mesh, output_mesh;
			if (vcg::tri::io::Importer<MyMesh>::Open(mesh, entry.path().string().c_str()) != 0) {
				std::cerr << "Error reading file: " << entry.path().string() << std::endl;
				logFile << "Error reading file: " << entry.path().string() << std::endl;
				continue;
			}
			std::cout << "Read mesh " << entry.path().string() << " successfully with " << mesh.VN() << " vertices" << std::endl;
			logFile << "Read mesh " << entry.path().string() << " successfully" << std::endl;

			// Remesh the mesh
			remesh_mesh(mesh, output_mesh);

			std::cout << "Saving mesh to file: " << m_output_dir << "/" << entry.path().filename().string() << std::endl;
			logFile << "Saving mesh to file: " << m_output_dir << "/" << entry.path().filename().string() << std::endl;

			if (vcg::tri::io::Exporter<MyMesh>::Save(output_mesh, outputFile.c_str()) != 0) {
				std::cerr << "Error writing file: " << outputFile << std::endl;
				logFile << "Error writing file: " << outputFile << std::endl;
			}

			
		}

		logFile.close();
	}
}

void VCGremesh::remesh_directory() {
	// Iterate over all .obj files in the directory and remesh the meshes inside it until it has the target number of vertices
	std::filesystem::path directoryPath(m_input_dir);
	std::string dirName = directoryPath.filename().string();

	std::string logFileName = m_output_dir + "/" + dirName + "/remeshLog.txt";
	std::ofstream logFile(logFileName);

	for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
		if (entry.path().extension() != ".obj") {
			continue;
		}
		
		std::string objName = entry.path().filename().string();
		// Skip mesh if it already exists in output folder
		if (std::filesystem::exists(m_output_dir + "/" + dirName + "/" + objName)) {
			std::cout << "File: " << m_output_dir + "/" + dirName + "/" + objName << " already exists, skipping..." << std::endl;
			logFile << "File: " << m_output_dir + "/" + dirName + "/" + objName << " already exists, skipping..." << std::endl;
			continue;
		}

		if (m_to_skip.find(dirName + "/" + objName) != m_to_skip.end()) {
			std::cout << "Skipping mesh: " << objName << std::endl;
			logFile << "Skipping mesh: " << objName << std::endl;
			continue;
		}

		// Load the mesh
		MyMesh mesh, output_mesh;
		if (vcg::tri::io::Importer<MyMesh>::Open(mesh, entry.path().string().c_str()) != 0) {
			std::cerr << "Error reading file: " << entry.path().string() << std::endl;
			logFile << "Error reading file: " << entry.path().string() << std::endl;
			continue;
		}
		std::cout << "Read mesh " << entry.path().string() << " successfully with " << mesh.VN() << " vertices" << std::endl;
		logFile << "Read mesh " << entry.path().string() << " successfully" << std::endl;

		//remesh_mesh(mesh);
		//resample_mesh(mesh, output_mesh); // This one creates holes...
		remesh_mesh(mesh, output_mesh);
		//increase_vertices(mesh, output_mesh);


		std::cout << "Saving mesh to file: " << m_output_dir << "/" << entry.path().filename().string() << std::endl;
		logFile << "Saving mesh to file: " << m_output_dir << "/" << entry.path().filename().string() << std::endl;

		// Get output file name 
		std::filesystem::path filePath(entry);
		std::string fileName = filePath.filename().string();
		std::string dirName = filePath.parent_path().filename().string();
		std::string outputFileName = m_output_dir + "/" + dirName + "/" + fileName;

		if (vcg::tri::io::Exporter<MyMesh>::Save(output_mesh, outputFileName.c_str()) != 0) {
			std::cerr << "Error writing file: " << outputFileName << std::endl;
			logFile << "Error writing file: " << outputFileName << std::endl;
		}
	}
}