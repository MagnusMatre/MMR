#include "TestMesh.h"

#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Renderer.h"

#include <iostream>
#include <filesystem>


namespace test {

	TestMesh::TestMesh()
		: m_lightcolor(glm::vec3(1.0f, 1.0f, 1.0f)), m_ambientlightcolor(glm::vec3(0.2f, 0.2f, 0.2f)),
		m_translationLight(3.0f, 3.0f, 3.0f), m_translationMesh(0.0f, 0.0f, 0.0f),
		m_projection(glm::perspective(glm::radians(45.0f), (float)800 / (float)800, 0.1f, 100.0f)),
		m_keyPressed(false), m_renderMode(RenderMode::WireframeAndSolid), m_translationCamera(0.0f, 0.0f, 0.0f),
		m_curModelName(""), m_mvp(glm::mat4(1.0f)), m_view(glm::mat4(1.0f)), m_wireFrameColor(glm::vec3(1.0f, 0.0f, 0.0f)), m_wireFrameWidth(1.0f)	
	{

		GLCall(glEnable(GL_BLEND));
		GLCall(glDepthFunc(GL_LESS));
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		GLCall(glEnable(GL_CULL_FACE)); // Enable face culling
		GLCall(glCullFace(GL_BACK));    // Cull back faces
		GLCall(glFrontFace(GL_CCW));    // Define front faces as counter-clockwise

		// LOAD MESHES

		std::string modelDirectory = "../../data/MeshesFiltered/Bird";

		for (const auto& entry : std::filesystem::directory_iterator(modelDirectory)) {
			if (m_curModelName == "") {
				m_curModelName = entry.path().string();
				m_models.push_back(Model(m_curModelName));
			}
			m_files.push_back(entry.path().string());
		}

		m_renderer = std::make_unique<Renderer>();

		m_shaderMeshSolid = std::make_unique<Shader>("resources/shaders/MeshSolid.shader");
		m_shaderMeshWireframe = std::make_unique<Shader>("resources/shaders/MeshWireframe.shader");
	}

	TestMesh::~TestMesh() {

	}

	void TestMesh::SetWindow(GLFWwindow* window) {
		m_window = window;
	}

	void TestMesh::InitializeCamera() {
		m_camera = std::make_unique<Camera>(m_window, glm::vec3(0.0f, 0.0f, 5.0f));
	}

	void TestMesh::OnUpdate(GLFWwindow* window, float deltaTime) {
		if (m_camera == nullptr) {
			InitializeCamera();
		}

		m_camera->ProcessKeyboard(deltaTime);
		m_view = m_camera->GetViewMatrix();
		m_projection = m_camera->GetProjectionMatrix(800.0f / 800.0f);

		// Change the rendering mode (might change this to different shaders)
		if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !m_keyPressed) {
			m_keyPressed = true;
			// Change the rendering mode
			if (m_renderMode == RenderMode::WireframeAndSolid) {
				m_renderMode = RenderMode::OnlyWireframe;
			}
			else if (m_renderMode == RenderMode::OnlyWireframe) {
				m_renderMode = RenderMode::OnlySolid;
			}
			else if (m_renderMode == RenderMode::OnlySolid) {
				m_renderMode = RenderMode::WireframeAndSolid;
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
			m_keyPressed = false;
		}
	}

	void TestMesh::OnRender() {

		glm::mat4 modelMesh = glm::translate(glm::mat4(1.0f), m_translationMesh);
		m_mvp = m_projection * m_view * modelMesh;
		if (m_renderMode == RenderMode::OnlySolid) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			m_shaderMeshSolid->Bind();
			m_shaderMeshSolid->SetUniformMat4f("u_MVP", m_mvp);
			m_shaderMeshSolid->SetUniformMat4f("u_model", modelMesh);
			m_shaderMeshSolid->SetUniform3f("u_lightcolor", m_lightcolor.x, m_lightcolor.y, m_lightcolor.z);
			m_shaderMeshSolid->SetUniform3f("u_ambientlightcolor", m_ambientlightcolor.x, m_ambientlightcolor.y, m_ambientlightcolor.z);
			m_shaderMeshSolid->SetUniform3f("u_lightpos", m_translationLight.x, m_translationLight.y, m_translationLight.z);
			m_shaderMeshSolid->SetUniform3f("u_viewpos", m_camera->Position.x, m_camera->Position.y, m_camera->Position.z);
			for (auto& model : m_models) {
				model.Draw(*m_shaderMeshSolid, *m_renderer);
			}
			m_shaderMeshSolid->Unbind();
		}
		else if (m_renderMode == RenderMode::OnlyWireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			m_shaderMeshWireframe->Bind();
			m_shaderMeshWireframe->SetUniformMat4f("u_MVP", m_mvp);
			m_shaderMeshWireframe->SetUniform3f("u_wireframecolor", m_wireFrameColor.x, m_wireFrameColor.y, m_wireFrameColor.z);
			m_shaderMeshWireframe->SetUniform1f("u_wireframewidth", m_wireFrameWidth);
			for (auto& model : m_models) {
				model.Draw(*m_shaderMeshWireframe, *m_renderer);
			}
			m_shaderMeshWireframe->Unbind();
		}
		else if (m_renderMode == RenderMode::WireframeAndSolid) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			m_shaderMeshSolid->Bind();
			m_shaderMeshSolid->SetUniformMat4f("u_MVP", m_mvp);
			m_shaderMeshSolid->SetUniformMat4f("u_model", modelMesh);
			m_shaderMeshSolid->SetUniform3f("u_lightcolor", m_lightcolor.x, m_lightcolor.y, m_lightcolor.z);
			m_shaderMeshSolid->SetUniform3f("u_ambientlightcolor", m_ambientlightcolor.x, m_ambientlightcolor.y, m_ambientlightcolor.z);
			m_shaderMeshSolid->SetUniform3f("u_lightpos", m_translationLight.x, m_translationLight.y, m_translationLight.z);
			m_shaderMeshSolid->SetUniform3f("u_viewpos", m_camera->Position.x, m_camera->Position.y, m_camera->Position.z);
			for (auto& model : m_models) {
				model.Draw(*m_shaderMeshSolid, *m_renderer);
			}
			m_shaderMeshSolid->Unbind();

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 1.0f);
			m_shaderMeshWireframe->Bind();
			m_shaderMeshWireframe->SetUniformMat4f("u_MVP", m_mvp);
			m_shaderMeshWireframe->SetUniform3f("u_wireframecolor", m_wireFrameColor.x, m_wireFrameColor.y, m_wireFrameColor.z);
			m_shaderMeshWireframe->SetUniform1f("u_wireframewidth", m_wireFrameWidth);
			for (auto& model : m_models) {
				model.Draw(*m_shaderMeshWireframe, *m_renderer);
			}
			m_shaderMeshWireframe->Unbind();
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
			}
		}

	}

}
