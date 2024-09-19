#pragma once

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include "assimp/Importer.hpp"
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

// Vertex strcuture for OpenGL
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
};

// Vertex structure for Mesh computation
struct MeshVertex {
	glm::vec3 position;
	std::vector<int> adjacentVertices; // Indices of adjacent vertices
};

// Edge structure
struct MeshEdge {
	int v0, v1; // Indices of the vertices that form the edge
	int newVertexIndex; // Index of the new vertex created on this edge
};

// Triangle structure
struct MeshTriangle {
	int v0, v1, v2; // Indices of the vertices that form the triangle
};

class Mesh {
public:
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;

	std::vector<MeshVertex> m_meshVertices;
	std::vector<MeshTriangle> m_meshTriangles;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	void LoadMesh(const std::string& path, std::vector<MeshVertex>& vertices, std::vector<MeshTriangle>& triangles);

	void Draw(Shader& shader, Renderer& renderer);
private:
	std::unique_ptr<VertexBuffer> m_vb;
	std::unique_ptr<IndexBuffer> m_ib;
	std::unique_ptr<VertexArray> m_va;

	void setupMesh();
};