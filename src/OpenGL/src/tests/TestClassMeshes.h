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

namespace test {

	class TestClassMeshes : public Test
	{
	public:
		TestClassMeshes();
		~TestClassMeshes();

		void SetWindow(GLFWwindow* window);
		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		void InitializeCamera();
		void loadNewMesh(const std::string& path);
		void loadCurrentDirectory();

		void ProceedToNextMesh();

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
		std::string m_messyMeshesDirectory; // Path to save the messy meshes to
		std::vector<std::string> m_curFiles; // List of files in the current directory
		std::vector<std::string> m_curDirectories; // List of directories in the current directory
		std::string m_curModelName; // Name of the current model

		bool m_pmp_check = true;

		RenderMode m_renderMode;
		bool m_keyPressed = false;

		bool m_testStructure = true;
		float m_dotSize = 5.0f;
		glm::vec3 m_edgeColor = glm::vec3(0.0f, 0.0f, 1.0f);

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