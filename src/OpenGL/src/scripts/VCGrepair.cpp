#include "VCGrepair.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_set>

#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/hole.h>

//#include <vcg/simplex/face/topology.h>
//#include <vcg/simplex/face/component.h>

#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/topology.h>

#include <vcg/complex/algorithms/point_sampling.h>
#include <vcg/simplex/face/component_polygon.h>

VCGrepair::VCGrepair(std::string& input_dir, std::string& output_dir){
	m_input_dir = input_dir;
    m_output_dir = output_dir;

}

VCGrepair::~VCGrepair() {
}

void VCGrepair::Execute() {
	// Code to run VCGrepair
	std::cout << "Running VCGrepair" << std::endl;
	//repairMesh();
	repair_directory();
}

void VCGrepair::repairMesh(MyMesh& mesh) {
	// Remove duplicate vertices
	vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh);

	// Remove unreferenced vertices
	vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(mesh);

	// Remove degenerate faces
	vcg::tri::Clean<MyMesh>::RemoveDegenerateFace(mesh);

	//Update topology
	vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
	vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);


	int nonManifoldVertexFFCount = vcg::tri::Clean<MyMesh>::CountNonManifoldVertexFF(mesh);
	int nonManifoldEdgeFFCount = vcg::tri::Clean<MyMesh>::CountNonManifoldEdgeFF(mesh);

	std::cout << "BEFORE: Non manifold vertex count: " << nonManifoldVertexFFCount << std::endl;
	std::cout << "BEFORE: Non manifold edgeFF count: " << nonManifoldEdgeFFCount << std::endl;

	// Ensure the mesh is manifold
	//vcg::tri::Clean<MyMesh>::SplitManifoldComponents(mesh);
	vcg::tri::Clean<MyMesh>::RemoveNonManifoldVertex(mesh);
	vcg::tri::Clean<MyMesh>::RemoveNonManifoldFace(mesh);
	vcg::tri::Clean<MyMesh>::RemoveNonManifoldVertex(mesh);

	// Iterate over all pairs of triangles and make sure that adjacent faces have the same orientation, if not reorient the face
	//reorientFaces(mesh); // HIGHLY EXPERIMENTAL
	//vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
	//vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);

	nonManifoldVertexFFCount = vcg::tri::Clean<MyMesh>::CountNonManifoldVertexFF(mesh);
	nonManifoldEdgeFFCount = vcg::tri::Clean<MyMesh>::CountNonManifoldEdgeFF(mesh);

	std::cout << "AFTER: Non manifold vertex count: " << nonManifoldVertexFFCount << std::endl;
	std::cout << "AFTER: Non manifold edgeFF count: " << nonManifoldEdgeFFCount << std::endl;

	// Compact the mesh
	vcg::tri::Allocator<MyMesh>::CompactEveryVector(mesh);

	// Update the mesh topology
	vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
	vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);

	// Update the bounding box
	vcg::tri::UpdateBounding<MyMesh>::Box(mesh);

	// Update vertex normals
	vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalized(mesh);

	// Update face normals
	vcg::tri::UpdateNormal<MyMesh>::PerFaceNormalized(mesh);
	//vcg::tri::Hole<MyMesh>::EarCuttingFill(mesh);

	// Orient the mesh coherently
	bool is_oriented = false;
	bool is_orientable = true;
	vcg::tri::Clean<MyMesh>::OrientCoherentlyMesh(mesh, is_oriented, is_orientable);

	vcg::tri::Clean<MyMesh>::FlipNormalOutside(mesh);

	// Update the mesh topology
	vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
	vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);

}


void VCGrepair::repair_directory() {
	// Iterate over all .obj files in the directory and remesh the meshes inside it until it has the target number of vertices
	std::filesystem::path directoryPath(m_input_dir);
	//std::string logFileName = m_output_dir + "/repairLog.txt";
	//std::ofstream logFile(logFileName);

	for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
		if (entry.path().extension() != ".obj") {
			continue;
		}

		// Load the mesh
		MyMesh mesh;
		if (vcg::tri::io::Importer<MyMesh>::Open(mesh, entry.path().string().c_str()) != 0) {
			std::cerr << "Error reading file: " << entry.path().string() << std::endl;
			//logFile << "Error reading file: " << entry.path().string() << std::endl;
			continue;
		}
		std::cout << "Read mesh " << entry.path().string() << " successfully with " << "vertices" << std::endl;
		//logFile << "Read mesh " << entry.path().string() << " successfully" << std::endl;

		repairMesh(mesh);

		std::cout << "Saving mesh to file: " << " " << entry.path().string() << std::endl;
		//logFile << "Saving mesh to file: " << " " << entry.path().string() << std::endl;

		// Get output file name 
		std::filesystem::path filePath(entry);
		std::string fileName = filePath.filename().string();
		std::string directoryName = filePath.parent_path().filename().string();
		std::string output_name = m_output_dir + "/" + directoryName + "/" + fileName;
		std::cout << "Output file name: " << output_name << std::endl;

		if (vcg::tri::io::Exporter<MyMesh>::Save(mesh, output_name.c_str()) != 0) {
			std::cerr << "Error saving mesh file" << std::endl;
		}
		else {
			std::cout << "Saved mesh to file: " << output_name << std::endl;
		}
	}

	//logFile.close();
}


void VCGrepair::reorientFaces(MyMesh& mesh) {
	std::unordered_set<int> visited;  // To keep track of visited faces
	std::queue<int> faceQueue;        // Queue for BFS traversal

	// Start with the first face, assuming the mesh has at least one face
	if (mesh.face.size() == 0) return;
	faceQueue.push(0);  // Start with face 0
	visited.insert(0);

	while (!faceQueue.empty()) {
		int faceIndex = faceQueue.front();
		faceQueue.pop();
		MyFace& currentFace = mesh.face[faceIndex];

		// Traverse all adjacent faces of the current face
		for (int i = 0; i < 3; ++i) {
			MyFace* adjFace = currentFace.FFp(i);
			if (adjFace == nullptr) continue; // No adjacent face

			int adjFaceIndex = vcg::tri::Index(mesh, *adjFace);
			if (visited.find(adjFaceIndex) != visited.end()) continue; // Already visited

            // Check if the current face and adjacent face share the same orientation on the common edge
            int sharedEdgeIndex = currentFace.FFi(i);

			// Check if adjacent face has the same orientation as the current face along the shared edge
			if (sharedEdgeIndex != adjFace->FFi(adjFace->cVFi(faceIndex))) {
				// Flip the vertices of the ajdacent face
				std::cout << "Flipping edge " << adjFaceIndex << std::endl;
				std::swap(adjFace->V(0), adjFace->V(2));

				// Update the mesh topology
				vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);

			}

			// Mark the adjacent face as visited and push it to the queue
			visited.insert(adjFaceIndex);
			faceQueue.push(adjFaceIndex);
		}
	}
}