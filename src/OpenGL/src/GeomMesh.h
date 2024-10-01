#pragma once

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include "assimp/Importer.hpp"
#include "assimp/Exporter.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <vector>
#include <memory>

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Renderer.h"

// Vertex structure for Mesh computation
struct MeshVertex {
	glm::vec3 position;
	std::vector<int> adjacentVertices; // Indices of adjacent vertices
	std::vector<int> adjacentEdges; // Indices of incident edges
	std::vector<int> adjacentTriangles; // Indices of incident triangles
};

// Normal structure for Mesh computation
struct MeshNormal {
	glm::vec3 normal;
};

// Edge structure
struct MeshEdge {
	int v0, v1; // Indices of the vertices that form the edge
};

// Triangle structure
struct MeshTriangle {
	int v0, v1, v2; // Indices of the vertices that form the triangle
};


class GeomMesh {
public:
	std::vector<MeshVertex> m_meshVertices;
	std::vector<MeshNormal> m_meshVertexNormals;
	std::vector<MeshTriangle> m_meshTriangles;
	std::vector<MeshEdge> m_meshEdges;

	//Buffers for the mesh
	std::unique_ptr<VertexArray> m_vao;
	std::unique_ptr<VertexBuffer> m_vbo;
	std::unique_ptr<IndexBuffer> m_ibo;

	// Buffers for incident triangles
	std::unique_ptr<VertexArray> m_incidentTrianglesVAO;
	std::unique_ptr<VertexBuffer> m_incidentTrianglesVBO;
	std::unique_ptr<IndexBuffer> m_incidentTrianglesIBO;

	// Buffers for storing incident vertices
	std::unique_ptr<VertexArray> m_incidentVerticesVAO;
	std::unique_ptr<VertexBuffer> m_incidentVerticesVBO;
	std::unique_ptr<IndexBuffer> m_incidentVerticesIBO;

	// Buffers for storing incident edges
	/*std::unique_ptr<VertexArray> m_incidentEdgesVAO;
	std::unique_ptr<VertexBuffer> m_incidentEdgesVBO;
	std::unique_ptr<IndexBuffer> m_incidentEdgesIBO;*/

	// Data structure testing variables
	int m_selectedVertex;

	GeomMesh(const std::string& path, bool vaidate = false);
	void SaveMesh(const std::string& path);
	void DrawMesh(Shader& shader, Renderer& renderer);

	// Methods to update the buffers for drawable components of the mesh
	void UpdateMeshBuffers();
	void UpdateIncidentTriangleBuffers();
	void UpdateIncidentVertexBuffers();
	//void UpdateIncidentEdgeBuffers();

	// Method to draw incident triangles
	void DrawIncidentTriangles(Shader& shader, Renderer& renderer);

	void PrintStats(); // Method that prints the number of vertices, edges and triangles in the 

	void SimpleRefineLoop(); // Method that refines the mesh using the simple method
	void LaplacianRefineLoop(); // Method that refines the mesh using the Laplacian method
	void IncrementalMeshDecimation(int targetVertices); // Method that reduces the number of vertices in the mesh to the target number
	void FlipFaces(); // Method that flips the faces of the mesh such that all normals point in the same direction

	// Methods for testing data structure
	void SelectRandomVertex(); // Method that selects a vertex for testing

private:
	std::string m_validationOutputFolder; // Path to the output file for the validation

	void validateDuplicates(const std::string& path); // Method that validates the mesh, checks for duplicate vertices, edges and triangles
	void validateLocalOrientation(const std::string& path); // Method that validates the mesh, checks for local orientation of the triangles

	void getAdjacentVertices();
	void getEdges();
	void computeIncidentEdgesTriangles();
	void computeNormals();

};