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
#include "deque"
//#include "Camera.h"

//enum class RenderMode {
//	OnlyWireframe,
//	OnlySolid,
//	WireframeAndSolid
//};

namespace test {

	class TestMesh : public Test
	{
	public:
		TestMesh();
		~TestMesh();

		void SetWindow(GLFWwindow* window) override;
		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnRender() override;
		void CaptureSnapshots();
		void RenderModel();
		void SaveSnapshot(const std::string& filename, int width, int height);
		void SwitchModel(const std::string& modelPath);
		void OnImGuiRender() override;
	private:
		void InitializeCamera();

		GLFWwindow* m_window;
		std::unique_ptr<Renderer> m_renderer;

		std::vector<Model> m_models;

		std::unique_ptr<Shader> m_shaderMeshSolid;
		std::unique_ptr<Shader> m_shaderMeshWireframe;
		std::unique_ptr<Shader> m_shaderMeshWireframeSolid;

		std::deque<std::string> m_files;
		std::vector<std::string> directory_filename;
		std::string m_curModelName;
		std::string m_curModelObjectName;
		std::string m_root_directory;
		std::string m_current_directory_filename;
		Model* m_curModel;
		RenderMode m_renderMode;
		bool m_keyPressed = false;
		bool m_snapshotted;

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

		//static void on_mouse_callback(GLFWwindow* window, double xpos, double ypos)
		//{

		//}

		//static void on_zoom_callback(GLFWwindow* window, double xoffset, double yoffset)
		//{

		//}
	};
}