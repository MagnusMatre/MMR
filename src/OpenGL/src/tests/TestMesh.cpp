#include "TestMesh.h"

#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Renderer.h"

#include <iostream>
#include <filesystem>


//void on_zoom_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//
//}
//
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//	if (key == GLFW_KEY_E && action == GLFW_PRESS)
//		std::cout << "Pressed E" << std::endl;
//}


namespace test {

	TestMesh::TestMesh()
		: m_lightcolor(glm::vec3(1.0f, 1.0f, 1.0f)), m_ambientlightcolor(glm::vec3(0.2f, 0.2f, 0.2f)),
		m_translationLight(3.0f, 3.0f, 3.0f), m_translationMesh(0.0f, 0.0f, 0.0f),
		m_projection(glm::perspective(glm::radians(45.0f), (float)800 / (float)800, 0.1f, 100.0f))
	{

		GLCall(glEnable(GL_BLEND));
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		// LOAD MESHES

		std::string modelDirectory = "../../data/Meshes/AircraftBuoyant";

		for (const auto& entry : std::filesystem::directory_iterator(modelDirectory)) {
			if (m_curModelName == "") {
				m_curModelName = entry.path().string();
				m_models.push_back(Model(m_curModelName));
			}
			m_files.push_back(entry.path().string());
		}



		//std::string modelPath = "../../data/Meshes/AircraftBuoyant/m1338.obj";
		//m_models.push_back(Model(m_files[0]));

		m_renderer = std::make_unique<Renderer>();

		m_shaderMesh = std::make_unique<Shader>("resources/shaders/Mesh.shader");
		m_shaderMesh->Bind();
		m_shaderMesh->Unbind();

		//m_shaderLight = std::make_unique<Shader>("resources/shaders/LightSource.shader");
		//m_shaderLight->Bind();
		//m_shaderLight->Unbind();
	}

	TestMesh::~TestMesh() {

	}

	void TestMesh::InitializeCamera(GLFWwindow* window) {
		m_camera = std::make_unique<Camera>(window, glm::vec3(0.0f, 0.0f, 5.0f));
	}

	void TestMesh::OnUpdate(GLFWwindow* window, float deltaTime) {
		m_camera->ProcessKeyboard(deltaTime);
		m_view = m_camera->GetViewMatrix();
	}

	void TestMesh::OnRender() {

		{ // Draw mesh
			glm::mat4 modelMesh = glm::translate(glm::mat4(1.0f), m_translationMesh);
			m_mvp = m_projection * m_view * modelMesh;
			m_shaderMesh->Bind();
			m_shaderMesh->SetUniformMat4f("u_MVP", m_mvp);
			m_shaderMesh->SetUniformMat4f("u_model", modelMesh);
			m_shaderMesh->SetUniform3f("u_lightcolor", m_lightcolor.x, m_lightcolor.y, m_lightcolor.z);
			m_shaderMesh->SetUniform3f("u_ambientlightcolor", m_ambientlightcolor.x, m_ambientlightcolor.y, m_ambientlightcolor.z);
			m_shaderMesh->SetUniform3f("u_lightpos", m_translationLight.x, m_translationLight.y, m_translationLight.z);
			m_shaderMesh->SetUniform3f("u_viewpos", m_camera->Position.x, m_camera->Position.y, m_camera->Position.z);
			for (auto& model : m_models) {
				model.Draw(*m_shaderMesh, *m_renderer);
			}
			//m_curModel->Draw(*m_shaderMesh, *m_renderer);
		}

	}

	void TestMesh::OnImGuiRender() {
		ImGui::SliderFloat3("Translation light source", &m_translationLight.x, -5.0f, 5.0f);
		ImGui::SliderFloat3("Translation mesh", &m_translationMesh.x, -5.0f, 5.0f);
		ImGui::SliderFloat3("Translation camera", &m_translationCamera.x, -5.0, 5.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Text("Camera:");
		ImGui::Text("X.pos: %.3f, Y.pos: %.3f, Z.pos: %.3f ", m_camera->Position.x, m_camera->Position.y, m_camera->Position.z);
		if (ImGui::Button("RESET CAMERA")) {
			m_camera->Position = glm::vec3(0.0f, 0.0f, 5.0f);
			m_camera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
			m_camera->Yaw = -90.0f;
			m_camera->Pitch = 0.0f;
		}

		std::string textToDisplay = "Choose model: (current: " + m_curModelName + ")";
		ImGui::Text(textToDisplay.c_str());
		for (auto& file : m_files) {
			if (ImGui::Button(file.c_str())) {
				m_curModelName = file;
				m_models.clear();
				m_models.push_back(Model(m_curModelName));
				//m_curModel = Model(m_curModelName);
			}
		}

	}

}
