#include "Mesh.h"

#include <iostream>

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) :
	m_vertices(vertices), m_indices(indices)
{
	setupMesh();
}

void Mesh::Draw(Shader& shader, Renderer& renderer) {
	// We don't really need the Draw call wrapper, just explicitly write it in the test OnRender function similar to TestCubes.cpp
	renderer.Draw(*m_va, *m_ib, shader, GL_TRIANGLES);
}

void Mesh::setupMesh() {
	std::cout << "Size of vertex: " << sizeof(Vertex) << " Number of vertices: " << m_vertices.size() << std::endl;
	m_va = std::make_unique<VertexArray>();
	m_vb = std::make_unique<VertexBuffer>(&m_vertices[0], sizeof(Vertex) * m_vertices.size());

	VertexBufferLayout layout;
	layout.Push<float>(3); // Positions
	layout.Push<float>(3); // Normals
	m_va->AddBuffer(*m_vb, layout);

	m_ib = std::make_unique<IndexBuffer>(&m_indices[0], m_indices.size());
	m_ib->Unbind();

}