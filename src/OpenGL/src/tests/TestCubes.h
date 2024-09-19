#pragma once

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include "Test.h"

#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
//#include "Camera.h"

namespace test {

	class TestCubes : public Test
	{
	public:
		TestCubes();
		~TestCubes();

		void SetWindow(GLFWwindow* window) override;
		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		void InitializeCamera();

		GLFWwindow* m_window;
		std::unique_ptr<Renderer> m_renderer;

		std::unique_ptr<IndexBuffer> m_ib;
		std::unique_ptr<VertexBuffer> m_vb;
		std::unique_ptr<VertexArray> m_va;
		std::unique_ptr<Shader> m_shaderCube;
		std::unique_ptr<Shader> m_shaderLight;

		std::unique_ptr<Camera> m_camera;
		
		//glm::mat4 m_proj;
		glm::mat4 m_view;
		glm::mat4 m_projection;
		
		// Model matrix translations
		glm::vec3 m_translationLight;
		glm::vec3 m_translationCube;
		glm::vec3 m_translationCamera;

		glm::mat4 m_mvp;

		glm::vec3 m_cubecolor;
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