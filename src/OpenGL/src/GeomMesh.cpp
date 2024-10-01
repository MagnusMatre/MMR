#include "GeomMesh.h"
#include <iostream>
#include <fstream>
#include <queue>

/*
METHODS TO IMPLEMENT:
- More advanced mesh refinement algorithms:
    - Laplacian smoothing
    - Advanced simple mesh refinement
- Mesh correction algorithms, i.e., flipping faces, removing degenerate faces, etc.
- Mesh simplification algorithms, i.e., edge collapse, vertex decimation, etc.

CHECKS TO DO:
- Check how long it takes for the GeomMesh to load in the largest mesh from the data folder (>20s). If it takes too long, consider optimizing the code.

TESTS SCRIPTS TO IMPLEMENT:
- Validate all of the meshes once for duplicates, also check construction time
- Check if the data structures of the mesh are correctly initialized
- Check if the simple refinement algorithm updates the data structures properly
- ... (more to come)
*/


// Problem: construct takes >30s for large meshes
GeomMesh::GeomMesh(const std::string& path, bool validate) : 
    m_validationOutputFolder("../../res/validation_checks"), m_selectedVertex(0) {
    Assimp::Importer importer;

    // Read the file
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    // Check the number of meshes in the scene
    if (scene->mNumMeshes > 1) {
        std::cerr << "WARNING::ASSIMP::Multiple meshes found in the scene. Only refining the first mesh..." << std::endl;
    }

    // Process the first mesh in the scene
    aiMesh* mesh = scene->mMeshes[0];

    // Convert vertices
    m_meshVertices.reserve(mesh->mNumVertices);

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        aiVector3D aiVertex = mesh->mVertices[i];
        MeshVertex vertex;
        vertex.position = glm::vec3(aiVertex.x, aiVertex.y, aiVertex.z);
        m_meshVertices.push_back(vertex);
    }

    if (validate) {
        validateDuplicates(path);
        validateLocalOrientation(path);
    }

    // Convert triangles and compute normals
    m_meshTriangles.reserve(mesh->mNumFaces);

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        if (face.mNumIndices != 3) {
            std::cerr << "ERROR::ASSIMP::Mesh is not triangulated." << std::endl;
            continue;
        }
        MeshTriangle triangle;
        triangle.v0 = face.mIndices[0];
        triangle.v1 = face.mIndices[1];
        triangle.v2 = face.mIndices[2];
        m_meshTriangles.push_back(triangle);

        // Compute the normal of the triangle
        MeshVertex& v0 = m_meshVertices[triangle.v0];
        MeshVertex& v1 = m_meshVertices[triangle.v1];
        MeshVertex& v2 = m_meshVertices[triangle.v2];
    }

    // Normalize the accumulated normals (don't normalize for speedup?)
    for (auto& normal : m_meshVertexNormals) {
		normal.normal = glm::normalize(normal.normal); // NOTE this is not a weighted average with respect to are of incident triangles
    }

	// Compute the adjacent vertices of each vertex
	getAdjacentVertices();

	// Compute the edges of the mesh
	getEdges();

	// For each vertex, also compute incident edges and triangles
	computeIncidentEdgesTriangles();
    computeNormals();

	// Update buffers 
    std::cout << "Updating mesh bufffers..." << std::endl;
    UpdateMeshBuffers();
    //std::cout << "Updating incident edge bufffers..." << std::endl;
    //UpdateIncidentEdgeBuffers();
    //std::cout << "Updating incident triangle bufffers..." << std::endl;
	//UpdateIncidentTriangleBuffers();
    //std::cout << "Updating incident vertex bufffers..." << std::endl;
	//UpdateIncidentVertexBuffers();
}

void GeomMesh::getAdjacentVertices() {
    // Initialize the adjacency list
    std::vector<std::unordered_set<int>> adjacencyList(m_meshVertices.size());

    // Build the adjacency list
    for (const auto& triangle : m_meshTriangles) {
        adjacencyList[triangle.v0].insert(triangle.v1);
        adjacencyList[triangle.v0].insert(triangle.v2);
        adjacencyList[triangle.v1].insert(triangle.v0);
        adjacencyList[triangle.v1].insert(triangle.v2);
        adjacencyList[triangle.v2].insert(triangle.v0);
        adjacencyList[triangle.v2].insert(triangle.v1);
    }

    // Convert the adjacency list to the required format
    for (unsigned int i = 0; i < m_meshVertices.size(); ++i) {
        m_meshVertices[i].adjacentVertices.assign(adjacencyList[i].begin(), adjacencyList[i].end());
    }
}

void GeomMesh::getEdges() {
	// Compute the edges of the mesh
	for (unsigned int i = 0; i < m_meshTriangles.size(); ++i) {
		MeshTriangle triangle = m_meshTriangles[i];
		MeshEdge edge01 = { triangle.v0, triangle.v1 };
		MeshEdge edge12 = { triangle.v1, triangle.v2 };
		MeshEdge edge20 = { triangle.v2, triangle.v0 };
		m_meshEdges.push_back(edge01);
		m_meshEdges.push_back(edge12);
		m_meshEdges.push_back(edge20);
	}
}

void GeomMesh::computeIncidentEdgesTriangles() {
    // Initialize the adjacency lists
    std::vector<std::unordered_set<int>> incidentEdges(m_meshVertices.size());
    std::vector<std::unordered_set<int>> incidentTriangles(m_meshVertices.size());

    // Build the incident edges list
    for (unsigned int i = 0; i < m_meshEdges.size(); ++i) {
        const auto& edge = m_meshEdges[i];
        incidentEdges[edge.v0].insert(i);
        incidentEdges[edge.v1].insert(i);
    }

    // Build the incident triangles list
    for (unsigned int i = 0; i < m_meshTriangles.size(); ++i) {
        const auto& triangle = m_meshTriangles[i];
        incidentTriangles[triangle.v0].insert(i);
        incidentTriangles[triangle.v1].insert(i);
        incidentTriangles[triangle.v2].insert(i);
    }

    // Convert the incident edges and triangles lists to the required format
    for (unsigned int i = 0; i < m_meshVertices.size(); ++i) {
		m_meshVertices[i].adjacentEdges.clear();
		m_meshVertices[i].adjacentTriangles.clear();
        m_meshVertices[i].adjacentEdges.assign(incidentEdges[i].begin(), incidentEdges[i].end());
        m_meshVertices[i].adjacentTriangles.assign(incidentTriangles[i].begin(), incidentTriangles[i].end());
    }
}

void GeomMesh::validateDuplicates(const std::string& path) {
    // Validate the mesh
    std::string failReason = "";

    // Check for duplicate vertices
    for (unsigned int i = 0; i < m_meshVertices.size(); ++i) {
        for (unsigned int j = i + 1; j < m_meshVertices.size(); ++j) {
            if (m_meshVertices[i].position == m_meshVertices[j].position) {
                failReason = "Duplicate vertices found at indices: " + std::to_string(i) + " and " + std::to_string(j);
            }
        }
    }

    // Check for duplicate edges
    for (unsigned int i = 0; i < m_meshEdges.size(); ++i) {
        for (unsigned int j = i + 1; j < m_meshEdges.size(); ++j) {
            if ((m_meshEdges[i].v0 == m_meshEdges[j].v0 && m_meshEdges[i].v1 == m_meshEdges[j].v1) ||
                (m_meshEdges[i].v0 == m_meshEdges[j].v1 && m_meshEdges[i].v1 == m_meshEdges[j].v0)) {
                failReason = "Duplicate edges found at indices: " + std::to_string(i) + " and " + std::to_string(j);
            }
        }
    }

    // Check for duplicate triangles
    for (unsigned int i = 0; i < m_meshTriangles.size(); ++i) {
        for (unsigned int j = i + 1; j < m_meshTriangles.size(); ++j) {
            if ((m_meshTriangles[i].v0 == m_meshTriangles[j].v0 && m_meshTriangles[i].v1 == m_meshTriangles[j].v1 && m_meshTriangles[i].v2 == m_meshTriangles[j].v2) ||
                (m_meshTriangles[i].v0 == m_meshTriangles[j].v1 && m_meshTriangles[i].v1 == m_meshTriangles[j].v2 && m_meshTriangles[i].v2 == m_meshTriangles[j].v0) ||
                (m_meshTriangles[i].v0 == m_meshTriangles[j].v2 && m_meshTriangles[i].v1 == m_meshTriangles[j].v0 && m_meshTriangles[i].v2 == m_meshTriangles[j].v1)) {
                failReason = "Duplicate triangles found at indices: " + std::to_string(i) + " and " + std::to_string(j);
            }
        }
    }

    // Write the validation result to a file
    std::ofstream validationFile(m_validationOutputFolder + "/duplication.txt", std::ios::app); // Open the file in append mode
    if (validationFile.is_open()) {
        if (failReason == "") {
            validationFile << path << " SUCCESS"  << std::endl;
        } else {
            validationFile << path << " FAILED: " << failReason << std::endl;
        }
        validationFile.close();
        std::cout << "Validation result saved to: " << m_validationOutputFolder << "/duplication.txt" << std::endl;
    } else {
        std::cerr << "ERROR::GEOMMESH::Failed to open validation file: " << m_validationOutputFolder << "/duplication.txt" << std::endl;
    }
}


void GeomMesh::validateLocalOrientation(const std::string& path) {
    // Validate the mesh
    std::string failReason = "";

    float meshDirection = 0.0f;

    // Iterate over all connected components of the mesh
    std::unordered_map<int, bool> visitedTriangles;
    for (unsigned int i = 0; i < m_meshTriangles.size(); ++i) {
        float connectedCompDirection = 0.0f;

        if (visitedTriangles[i]) {
            continue; // Skip triangles that have already been handled
        }

        std::vector<int> componentTriangles;
        std::queue<int> triangleQueue;
        triangleQueue.push(i);

        // Perform breadth-first search to find all triangles in the connected component
        while (!triangleQueue.empty()) {
            int triangleIndex = triangleQueue.front();
            triangleQueue.pop();

            if (visitedTriangles[triangleIndex]) {
                continue; // Skip triangles that have already been visited
            }

            visitedTriangles[triangleIndex] = true;
            componentTriangles.push_back(triangleIndex);

            // Get the incident triangles of the current triangle
            const MeshTriangle& triangle = m_meshTriangles[triangleIndex];
            const std::vector<int>& incidentTriangles = m_meshVertices[triangle.v0].adjacentTriangles;

            // Add adjacent triangles to the queue
            for (int incidentTriangleIndex : incidentTriangles) {
                triangleQueue.push(incidentTriangleIndex);
            }
        }

        // Check the local orientation of the triangles in the connected component
        for (int triangleIndex : componentTriangles) {
            const MeshTriangle& triangle = m_meshTriangles[triangleIndex];
            const MeshVertex& v0 = m_meshVertices[triangle.v0];
            const MeshVertex& v1 = m_meshVertices[triangle.v1];
            const MeshVertex& v2 = m_meshVertices[triangle.v2];

            // Compute the normal of the triangle
            glm::vec3 normal = glm::cross(v1.position - v0.position, v2.position - v0.position);
            normal = glm::normalize(normal);

            float direction = glm::dot(normal, v0.position);

            // Assume no degenerate triangles (line segments)
            if (meshDirection == 0.0f) {
                meshDirection = direction;
                connectedCompDirection = direction;
            }
			else if (connectedCompDirection == 0.0f) {
				connectedCompDirection = direction;
			}
            else {
                // Check if the normal points in the right direction
                if (glm::dot(normal, v0.position) * meshDirection < 0.0f) { // If the product is negative, the normal points in the opposite direction
                    failReason = "Triangle at index: " + std::to_string(triangleIndex) + " has an incorrect (local) orientation";
                    break;
                }
				else if (glm::dot(normal, v0.position) * meshDirection) {
					failReason = "Triangle at index: " + std::to_string(triangleIndex) + " has an incorrect orientation with respect to mesh";
					break;

                }
            }
        }

        if (!failReason.empty()) {
            break; // Stop checking other connected components if a failure is found
        }
    }

    // Write the validation result to a file
    std::ofstream validationFile(m_validationOutputFolder + "/local_orientation.txt", std::ios::app); // Open the file in append mode
    if (validationFile.is_open()) {
        if (failReason.empty()) {
            validationFile << "Local orientation check SUCCESS" << std::endl;
        }
        else {
            validationFile << "Local orientation check FAILED: " << failReason << std::endl;
        }
        validationFile.close();
        std::cout << "Validation result saved to: " << m_validationOutputFolder << "/local_orientation.txt" << std::endl;
    }
    else {
        std::cerr << "ERROR::GEOMMESH::Failed to open validation file: " << m_validationOutputFolder << "/local_orientation.txt" << std::endl;
    }
}


void GeomMesh::PrintStats() {
    std::cout << "Number of vertices: " << m_meshVertices.size() << std::endl;
    std::cout << "Number of triangles: " << m_meshTriangles.size() << std::endl;
	std::cout << "Number of edges: " << m_meshEdges.size() << std::endl;
}

void GeomMesh::SimpleRefineLoop() {
    /*
		For each triangle, compute the midpoints of the edges and add them to the list of vertices.
		This means that the new vertex vector will store double the the number of vertices in the original mesh.
    */

    // Refine the mesh
    std::vector<MeshVertex> newVertices;
    std::vector<MeshTriangle> newTriangles;
    std::vector<MeshEdge> newEdges; // New edge list

    // Reserve space for the new vectors
	newVertices.reserve(m_meshVertices.size() + m_meshEdges.size());

    for (const auto& triangle : m_meshTriangles) {
        // Get the vertices of the triangle
        MeshVertex v0 = m_meshVertices[triangle.v0];
        MeshVertex v1 = m_meshVertices[triangle.v1];
        MeshVertex v2 = m_meshVertices[triangle.v2];

        // Compute the new vertices
        MeshVertex v01;
        MeshVertex v12;
        MeshVertex v20;

        v01.position = (v0.position + v1.position) / 2.0f;
        v12.position = (v1.position + v2.position) / 2.0f;
        v20.position = (v2.position + v0.position) / 2.0f;

        // Add the new vertices to the list
        newVertices.push_back(v0);
        newVertices.push_back(v1);
        newVertices.push_back(v2);
        newVertices.push_back(v01);
        newVertices.push_back(v12);
        newVertices.push_back(v20);

        // Add the new triangles to the list
        MeshTriangle t0 = { static_cast<int>(newVertices.size() - 6), static_cast<int>(newVertices.size() - 3), static_cast<int>(newVertices.size() - 1) };
        MeshTriangle t1 = { static_cast<int>(newVertices.size() - 3), static_cast<int>(newVertices.size() - 5), static_cast<int>(newVertices.size() - 2) };
        MeshTriangle t2 = { static_cast<int>(newVertices.size() - 1), static_cast<int>(newVertices.size() - 2), static_cast<int>(newVertices.size() - 4) };
        MeshTriangle t3 = { static_cast<int>(newVertices.size() - 3), static_cast<int>(newVertices.size() - 2), static_cast<int>(newVertices.size() - 1) };
        newTriangles.push_back(t0);
        newTriangles.push_back(t1);
        newTriangles.push_back(t2);
        newTriangles.push_back(t3);

        // Add the new edges to the list
        MeshEdge e0_01 = { static_cast<int>(newVertices.size() - 6), static_cast<int>(newVertices.size() - 3) };
        MeshEdge e01_1 = { static_cast<int>(newVertices.size() - 3), static_cast<int>(newVertices.size() - 5) };
        MeshEdge e1_12 = { static_cast<int>(newVertices.size() - 5), static_cast<int>(newVertices.size() - 6) };
        MeshEdge e12_2 = { static_cast<int>(newVertices.size() - 3), static_cast<int>(newVertices.size() - 6) };
        MeshEdge e2_02 = { static_cast<int>(newVertices.size() - 5), static_cast<int>(newVertices.size() - 4) };
        MeshEdge e02_0 = { static_cast<int>(newVertices.size() - 6), static_cast<int>(newVertices.size() - 4) };
        MeshEdge e01_12 = { static_cast<int>(newVertices.size() - 4), static_cast<int>(newVertices.size() - 1) };
        MeshEdge e12_02 = { static_cast<int>(newVertices.size() - 1), static_cast<int>(newVertices.size() - 2) };
        MeshEdge e02_01 = { static_cast<int>(newVertices.size() - 6), static_cast<int>(newVertices.size() - 2) };

        newEdges.push_back(e0_01);
        newEdges.push_back(e01_1);
        newEdges.push_back(e1_12);
        newEdges.push_back(e12_2);
        newEdges.push_back(e2_02);
        newEdges.push_back(e02_0);
        newEdges.push_back(e01_12);
        newEdges.push_back(e12_02);
        newEdges.push_back(e02_01);

    }

    // Update the mesh
    m_meshVertices = newVertices;
    m_meshTriangles = newTriangles;
    m_meshEdges = newEdges; // Update the edge list

    // Update the incident edges and triangles of the vertices
    computeIncidentEdgesTriangles();
	computeNormals(); // Note: compute normals after updating the incident edges and triangles

	// Update the buffers (must be done after normals are computed)
    UpdateMeshBuffers();
    //UpdateIncidentEdgeBuffers();
    UpdateIncidentTriangleBuffers();
    UpdateIncidentVertexBuffers();
}

void GeomMesh::computeNormals() {
	// Compute the normals of the vertices
	m_meshVertexNormals.reserve(m_meshVertices.size());
    m_meshVertexNormals.clear();
	m_meshVertexNormals.resize(m_meshVertices.size(), MeshNormal{ glm::vec3(0.0f) });

	for (const auto& vertex : m_meshVertices) {
        for (const auto& triangle : vertex.adjacentTriangles) {
			// Get the vertices of the triangle
			MeshVertex& v0 = m_meshVertices[m_meshTriangles[triangle].v0];
			MeshVertex& v1 = m_meshVertices[m_meshTriangles[triangle].v1];
			MeshVertex& v2 = m_meshVertices[m_meshTriangles[triangle].v2];

			// Compute the normal of the triangle
			glm::vec3 triangleNormal = glm::cross(v1.position - v0.position, v2.position - v0.position); // Don't normalize to take the area into account

			// Accumulate the weighted normal for each vertex of the triangle
			m_meshVertexNormals[m_meshTriangles[triangle].v0].normal += triangleNormal;
			m_meshVertexNormals[m_meshTriangles[triangle].v1].normal += triangleNormal;
			m_meshVertexNormals[m_meshTriangles[triangle].v2].normal += triangleNormal;
        }
	}

	// Normalize the accumulated normals
	for (auto& normal : m_meshVertexNormals) {
		normal.normal = glm::normalize(normal.normal);
	}
}


//Method that will save the mesh to a file: Each vertex takes a line in the file starting with v, and each face takes a line starting with f
void GeomMesh::SaveMesh(const std::string& path) {
	std::ofstream file(path);
	if (!file.is_open()) {
		std::cerr << "ERROR::GEOMMESH::Failed to open file: " << path << std::endl;
		return;
	}

	// Write the vertices
	for (const auto& vertex : m_meshVertices) {
		file << "v " << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << std::endl;
	}

	file << "# " << m_meshVertices.size() << " vertices" << std::endl;
    file << std::endl;

	// Write the faces
	for (const auto& triangle : m_meshTriangles) {
		file << "f " << triangle.v0 + 1 << " " << triangle.v1 + 1 << " " << triangle.v2 + 1 << std::endl;
	}
    file << "# " << m_meshVertices.size() << " faces" << std::endl;

	file.close();
	std::cout << "Saved mesh to: " << path << std::endl;
}

//Laplacian smoothing algorithms (UNTESTED)
void GeomMesh::LaplacianRefineLoop() {
	// Compute the new positions of the vertices
	std::vector<glm::vec3> newPositions(m_meshVertices.size());
	for (unsigned int i = 0; i < m_meshVertices.size(); ++i) {
		MeshVertex vertex = m_meshVertices[i];
		glm::vec3 sum = glm::vec3(0.0f);
		int count = 0;
		for (const auto& triangle : m_meshTriangles) {
			if (triangle.v0 == i || triangle.v1 == i || triangle.v2 == i) {
				MeshVertex v0 = m_meshVertices[triangle.v0];
				MeshVertex v1 = m_meshVertices[triangle.v1];
				MeshVertex v2 = m_meshVertices[triangle.v2];
				sum += v0.position + v1.position + v2.position;
				count += 3;
			}
		}
		newPositions[i] = sum / static_cast<float>(count);
	}

	// Update the positions of the vertices
	for (unsigned int i = 0; i < m_meshVertices.size(); ++i) {
		m_meshVertices[i].position = newPositions[i];
	}
}


//Mesh simplification algorithms (UNTESTED)
void GeomMesh::IncrementalMeshDecimation(int targetVertices) { 
	// Compute the cost of collapsing each edge
	std::vector<float> edgeCosts(m_meshEdges.size());
	for (unsigned int i = 0; i < m_meshEdges.size(); ++i) {
		MeshEdge edge = m_meshEdges[i];
		MeshVertex v0 = m_meshVertices[edge.v0];
		MeshVertex v1 = m_meshVertices[edge.v1];
		edgeCosts[i] = glm::length(v0.position - v1.position);
	}

	// Collapse the edge with the smallest cost
	while (m_meshVertices.size() > targetVertices) {
		// Find the edge with the smallest cost
		float minCost = std::numeric_limits<float>::max();
		unsigned int minIndex = 0;
		for (unsigned int i = 0; i < edgeCosts.size(); ++i) {
			if (edgeCosts[i] < minCost) {
				minCost = edgeCosts[i];
				minIndex = i;
			}
		}

		// Collapse the edge
		MeshEdge edge = m_meshEdges[minIndex];
		MeshVertex v0 = m_meshVertices[edge.v0];
		MeshVertex v1 = m_meshVertices[edge.v1];
		glm::vec3 newPosition = (v0.position + v1.position) / 2.0f;
		m_meshVertices[edge.v0].position = newPosition;

		// Update the adjacent vertices
		for (int adjacentVertex : v1.adjacentVertices) {
			if (adjacentVertex != edge.v0) {
				m_meshVertices[adjacentVertex].adjacentVertices.push_back(edge.v0);
			}
		}

		// Remove the edge and update the costs
		m_meshVertices.erase(m_meshVertices.begin() + edge.v1);
		m_meshEdges.erase(m_meshEdges.begin() + minIndex);
		edgeCosts.erase(edgeCosts.begin() + minIndex);
		for (unsigned int i = 0; i < edgeCosts.size(); ++i) {
			if (m_meshEdges[i].v0 == edge.v1 || m_meshEdges[i].v1 == edge.v1) {
				edgeCosts[i] = std::numeric_limits<float>::max();
			}
		}
	}
}


//Flip faces algorithm (UNTESTED)
void GeomMesh::FlipFaces() {
	// Compute the new triangles
	std::vector<MeshTriangle> newTriangles;
	for (const auto& triangle : m_meshTriangles) {
		MeshVertex v0 = m_meshVertices[triangle.v0];
		MeshVertex v1 = m_meshVertices[triangle.v1];
		MeshVertex v2 = m_meshVertices[triangle.v2];

		// Compute the normal of the triangle
			glm::vec3 normal = glm::cross(v1.position - v0.position, v2.position - v0.position);
		normal = glm::normalize(normal);

		// Compute the centroid of the triangle
		glm::vec3 centroid = (v0.position + v1.position + v2.position) / 3.0f;

		// Compute the new vertices
		MeshVertex v01;
		MeshVertex v12;
		MeshVertex v20;
        
		v01.position = (v0.position + v1.position) / 2.0f;
		v12.position = (v1.position + v2.position) / 2.0f;
		v20.position = (v2.position + v0.position) / 2.0f;

		// Compute the new triangles
		MeshTriangle t0 = { triangle.v0, static_cast<int>(m_meshVertices.size()), static_cast<int>(m_meshVertices.size() + 1) };
		MeshTriangle t1 = { triangle.v1, static_cast<int>(m_meshVertices.size() + 1), static_cast<int>(m_meshVertices.size() + 2) };
		MeshTriangle t2 = { triangle.v2, static_cast<int>(m_meshVertices.size() + 2), static_cast<int>(m_meshVertices.size()) };
		MeshTriangle t3 = { static_cast<int>(m_meshVertices.size()), static_cast<int>(m_meshVertices.size() + 1), static_cast<int>(m_meshVertices.size() + 2) };

		// Add the new vertices and triangles to the list
		newTriangles.push_back(t0);
		newTriangles.push_back(t1);
		newTriangles.push_back(t2);
		newTriangles.push_back(t3);
	}

	// Update the mesh
	m_meshTriangles = newTriangles;
	std::cout << "Flipped faces successfully: " << std::endl;
	std::cout << "New number of vertices: " << m_meshVertices.size() << std::endl;
	std::cout << "New number of faces: " << m_meshTriangles.size() << std::endl;
}

void GeomMesh::SelectRandomVertex() {
	m_selectedVertex = rand() % m_meshVertices.size();
	std::cout << "Selected vertex: " << m_selectedVertex << std::endl;

    //UpdateIncidentEdgeBuffers();
    UpdateIncidentTriangleBuffers();
    UpdateIncidentVertexBuffers();
}

void GeomMesh::UpdateMeshBuffers() {
	// Create the vertex array
	std::vector<float> vertices;
    for (unsigned int i = 0; i < m_meshVertices.size(); i++) {
		vertices.push_back(m_meshVertices[i].position.x);
		vertices.push_back(m_meshVertices[i].position.y);
		vertices.push_back(m_meshVertices[i].position.z);
        vertices.push_back(m_meshVertexNormals[i].normal.x);
        vertices.push_back(m_meshVertexNormals[i].normal.y);
        vertices.push_back(m_meshVertexNormals[i].normal.z);
	}

	m_vao = std::make_unique<VertexArray>();
	m_vbo = std::make_unique<VertexBuffer>(&vertices[0], sizeof(float) * vertices.size());
	VertexBufferLayout layout;
	layout.Push<float>(3);
    layout.Push<float>(3);
	m_vao->AddBuffer(*m_vbo, layout);

	// Create the index buffer (unfortunately need this extra buffer...)
    std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < m_meshTriangles.size(); ++i) {
		indices.push_back(m_meshTriangles[i].v0);
		indices.push_back(m_meshTriangles[i].v1);
		indices.push_back(m_meshTriangles[i].v2);
	}
	m_ibo = std::make_unique<IndexBuffer>(&indices[0], m_meshTriangles.size() * 3);
	m_ibo->Unbind();

}

//void GeomMesh::UpdateIncidentEdgeBuffers() {
//	// Find incident edges for the given vertex index
//	std::vector<float> vertices;
//	std::vector<unsigned int> indices;
//
//	// Find the vertices and indices to create the triangles
//	int index = 0;
//	for (int edgeIndex : m_meshVertices[m_selectedVertex].adjacentEdges) {
//		const MeshEdge& edge = m_meshEdges[edgeIndex];
//		const MeshVertex& v0 = m_meshVertices[edge.v0];
//		const MeshVertex& v1 = m_meshVertices[edge.v1];
//
//		// Add the vertices to the list
//		vertices.push_back(v0.position.x);
//		vertices.push_back(v0.position.y);
//		vertices.push_back(v0.position.z);
//		vertices.push_back(v1.position.x);
//		vertices.push_back(v1.position.y);
//		vertices.push_back(v1.position.z);
//
//		// Add the indices to the list
//		indices.push_back(index++);
//		indices.push_back(index++);
//	}
//
//	// LINES
//	// Create the vertex array
//	m_incidentEdgesVAO = std::make_unique<VertexArray>();
//	m_incidentEdgesVBO = std::make_unique<VertexBuffer>(&vertices[0], 3 * sizeof(float) * vertices.size());
//	VertexBufferLayout layout;
//	layout.Push<float>(3);
//	m_incidentEdgesVAO->AddBuffer(*m_incidentEdgesVBO, layout);
//
//	// Create the index buffer
//	m_incidentEdgesIBO = std::make_unique<IndexBuffer>(&indices[0], indices.size());
//	m_incidentEdgesIBO->Unbind();
//}


void GeomMesh::UpdateIncidentVertexBuffers() {
    // Find incident vertices for the given vertex index
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Find the vertices and indices to create the triangles
    int index = 0;
    for (int vertexIndex : m_meshVertices[m_selectedVertex].adjacentVertices) {
        const MeshVertex& vertex = m_meshVertices[vertexIndex];

        // Add the vertices to the list
        vertices.push_back(vertex.position.x);
        vertices.push_back(vertex.position.y);
        vertices.push_back(vertex.position.z);

        // Add the normals to the vertex buffer
        //glm::vec3 normal = glm::cross(v1.position - v0.position, v2.position - v0.position);
        //normal = glm::normalize(normal);
        //vertices.push_back(normal.x);
        //vertices.push_back(normal.y);
        //vertices.push_back(normal.z);

        // Add the indices to the list
        indices.push_back(index++);
    }

    // POINTS
    // Create the vertex array
    m_incidentVerticesVAO = std::make_unique<VertexArray>();
    m_incidentVerticesVBO = std::make_unique<VertexBuffer>(&vertices[0], 3 * sizeof(float) * vertices.size());
    VertexBufferLayout layout;
    layout.Push<float>(3);
    m_incidentVerticesVAO->AddBuffer(*m_incidentVerticesVBO, layout);

    // Create the index buffer
    m_incidentVerticesIBO = std::make_unique<IndexBuffer>(&indices[0], indices.size());
    m_incidentVerticesIBO->Unbind();
}

void GeomMesh::UpdateIncidentTriangleBuffers() {
    // Find incident triangles for the given vertex index
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Find the vertices and indices to create the triangles
    int index = 0;
	for (int triangleIndex : m_meshVertices[m_selectedVertex].adjacentTriangles) {
		const MeshTriangle& triangle = m_meshTriangles[triangleIndex];
		const MeshVertex& v0 = m_meshVertices[triangle.v0];
		const MeshVertex& v1 = m_meshVertices[triangle.v1];
		const MeshVertex& v2 = m_meshVertices[triangle.v2];

		// Add the vertices to the list
		vertices.push_back(v0.position.x);
		vertices.push_back(v0.position.y);
		vertices.push_back(v0.position.z);
		vertices.push_back(v1.position.x);
		vertices.push_back(v1.position.y);
		vertices.push_back(v1.position.z);
		vertices.push_back(v2.position.x);
		vertices.push_back(v2.position.y);
		vertices.push_back(v2.position.z);

        // Add the normals to the vertex buffer
		//glm::vec3 normal = glm::cross(v1.position - v0.position, v2.position - v0.position);
		//normal = glm::normalize(normal);
		//vertices.push_back(normal.x);
		//vertices.push_back(normal.y);
		//vertices.push_back(normal.z);
		//vertices.push_back(normal.x);
		//vertices.push_back(normal.y);
		//vertices.push_back(normal.z);
		//vertices.push_back(normal.x);
		//vertices.push_back(normal.y);
		//vertices.push_back(normal.z);

		// Add the indices to the list
		indices.push_back(index++);
		indices.push_back(index++);
		indices.push_back(index++);
	}

    // TRIANGLES
	// Create the vertex array
    m_incidentTrianglesVAO = std::make_unique<VertexArray>();
    m_incidentTrianglesVBO = std::make_unique<VertexBuffer>(&vertices[0], 3 * sizeof(float) * vertices.size());
	VertexBufferLayout layout;
	layout.Push<float>(3);
    m_incidentTrianglesVAO->AddBuffer(*m_incidentTrianglesVBO, layout);

	// Create the index buffer
    m_incidentTrianglesIBO = std::make_unique<IndexBuffer>(&indices[0], indices.size());
    m_incidentTrianglesIBO->Unbind();
}

void GeomMesh::DrawMesh(Shader& shader, Renderer& renderer) {
    // Draw the mesh
    shader.Bind();
    renderer.Draw(*m_vao, *m_ibo, shader, GL_TRIANGLES);
    shader.Unbind();
}

void GeomMesh::DrawIncidentTriangles(Shader& shader, Renderer& renderer) {
    //shader.Bind();
    //shader.SetUniform3f("u_color", 1.0f, 1.0f, 0.0f); // Yellow color
    //renderer.Draw(*m_incidentVAO, *m_incidentIBO, shader, GL_TRIANGLES);
    //shader.Unbind();
}
