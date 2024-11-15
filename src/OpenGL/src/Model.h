#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Mesh.h"
#include "Renderer.h"

#include <vector>
#include <string>

class Model
{
public:
    Model(std::string& path);
    void Draw(Shader& shader, Renderer& renderer);
private:
    // model data
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    //std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};