#pragma once

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include "Test.h"

#include "Model.h"
#include "Mesh.h"
#include "GeomMesh.h"
#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
//#include "Camera.h"

#include "FeatureExtraction.h"
#include "QueryEngine.h"

#include <memory> 

namespace test {

	class TestFeatures : public Test
	{
	public:
		TestFeatures();
		~TestFeatures();

		void SetWindow(GLFWwindow* window);
		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		void InitializeCamera();
		void loadNewMesh(const std::string& path);
		void loadCurrentDirectory();

		void loadCGALmesh(std::string& filename);
		void updateSampleVerticesBuffer();
		void loadSampleVertices();

		void computeDistances();

		std::string m_featureFile;
		std::unique_ptr<QueryEngine> m_queryEngine;

		CGALMesh m_CGALmesh;
		FeatureExtraction m_featureExtractor;
		std::string m_samplePointsDirectory;

		std::unique_ptr<VertexArray> m_sampledVerticesVAO;
		std::unique_ptr<VertexBuffer> m_sampledVerticesVBO;
		std::unique_ptr<IndexBuffer> m_sampledVerticesIBO;

		GLFWwindow* m_window;
		std::unique_ptr<Renderer> m_renderer;

		std::unique_ptr<Model> m_model;
		std::unique_ptr<GeomMesh> m_curGeomMesh;

		std::unique_ptr<Shader> m_shaderMeshSolid;
		std::unique_ptr<Shader> m_shaderMeshWireframe;
		std::unique_ptr<Shader> m_shaderMeshWireframeSolid;
		std::unique_ptr<Shader> m_shaderDot;


		// Variables for path handling
		std::string m_dataRoot; // Path to the root of the data folder

		std::string m_curDirectory; // Path to the current directory

		std::vector<std::string> m_curFiles; // List of files in the current directory
		std::vector<std::string> m_curDirectories; // List of directories in the current directory
		std::string m_curModelName; // Name of the current model

		RenderMode m_renderMode;
		bool m_keyPressed = false;

		std::unique_ptr<Camera> m_camera;

		//glm::mat4 m_proj;
		glm::mat4 m_view;
		glm::mat4 m_projection;

		// Model matrix translations
		glm::vec3 m_translationLight;
		glm::vec3 m_translationMesh;
		glm::vec3 m_translationCamera;

		glm::mat4 m_mvp;

		glm::vec3 m_lightcolor;
		glm::vec3 m_wireFrameColor;
		float m_wireFrameWidth;

		glm::vec3 m_ambientlightcolor;
	};
}