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
	renderer.Draw(*m_va, *m_ib, shader);
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

void Mesh::LoadMesh(const std::string& path, std::vector<MeshVertex>& m_meshVertices, std::vector<MeshTriangle>& m_meshTriangles) {
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

    // Convert triangles
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
    }
}