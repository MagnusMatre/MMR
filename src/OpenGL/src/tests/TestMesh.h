#pragma once

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include "Test.h"

#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
//#include "Camera.h"

namespace test {

	class TestMesh : public Test
	{
	public:
		TestMesh();
		~TestMesh();

		void InitializeCamera(GLFWwindow* window) override;
		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		std::unique_ptr<Renderer> m_renderer;

		std::vector<Model> m_models;

		std::unique_ptr<Shader> m_shaderMesh;
		std::vector<std::string> m_files;
		std::string m_curModelName;
		Model* m_curModel;

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

		glm::vec3 m_ambientlightcolor;

		//static void on_mouse_callback(GLFWwindow* window, double xpos, double ypos)
		//{

		//}

		//static void on_zoom_callback(GLFWwindow* window, double xoffset, double yoffset)
		//{

		//}
	};
}