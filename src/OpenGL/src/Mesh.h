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

class Mesh {
public:
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	void Draw(Shader& shader, Renderer& renderer);
private:
	std::unique_ptr<VertexBuffer> m_vb;
	std::unique_ptr<IndexBuffer> m_ib;
	std::unique_ptr<VertexArray> m_va;

	void setupMesh();
};