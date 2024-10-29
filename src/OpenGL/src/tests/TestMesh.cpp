#include "TestMesh.h"

#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Renderer.h"

#include <iostream>
#include <filesystem>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace test {


	TestMesh::TestMesh()
		: m_lightcolor(glm::vec3(1.0f, 1.0f, 1.0f)), m_ambientlightcolor(glm::vec3(0.2f, 0.2f, 0.2f)),
		m_translationLight(3.0f, 3.0f, 3.0f), m_translationMesh(0.0f, 0.0f, 0.0f),
		m_projection(glm::perspective(glm::radians(45.0f), (float)800 / (float)800, 0.1f, 100.0f)),
		m_keyPressed(false), m_renderMode(RenderMode::WireframeAndSolid), m_translationCamera(0.0f, 0.0f, 0.0f),
		m_curModelName(""), m_mvp(glm::mat4(1.0f)), m_view(glm::mat4(1.0f)), m_wireFrameColor(glm::vec3(0.0f, 0.0f, 0.0f)), m_wireFrameWidth(1.0f)
	{

		GLCall(glEnable(GL_BLEND));
		GLCall(glDepthFunc(GL_LESS));
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		GLCall(glEnable(GL_CULL_FACE)); // Enable face culling
		GLCall(glCullFace(GL_BACK));    // Cull back faces
		GLCall(glFrontFace(GL_CCW));    // Define front faces as counter-clockwise

		// LOAD MESHES

		//std::string modelDirectory = "output/Cellphone";
		m_root_directory = "C:/Users/Magnus/Documents/Master/MMR/VCGdecimatedCleaned/VCGdecimatedCleaned";


		for (const auto& entry : std::filesystem::directory_iterator(m_root_directory)) {
			auto class_name = entry.path().filename().string();
			for (const auto& file : std::filesystem::directory_iterator(m_root_directory + "/" + class_name)) {

				m_curModelName = file.path().string();
				m_curModelObjectName = file.path().stem().string();
				if (m_curModelObjectName == "remeshLog") continue;
				m_files.push_back(class_name + "/" + file.path().stem().string());
				std::cout << class_name + "/" + file.path().stem().string() << std::endl;
				//m_models.push_back(Model(m_curModelName));
				
			}
		}

		m_renderer = std::make_unique<Renderer>();

		m_shaderMeshSolid = std::make_unique<Shader>("resources/shaders/MeshSolid.shader");
		m_shaderMeshWireframe = std::make_unique<Shader>("resources/shaders/MeshWireframe.shader");
		m_snapshotted = true;
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
		if (m_snapshotted) {
			m_current_directory_filename = m_files.front();
			std::string path = (m_root_directory + "/" + m_current_directory_filename + ".obj");
			m_files.pop_front();
			std::cout << m_current_directory_filename << std::endl;
			m_models.emplace_back(Model(path));
			m_snapshotted = false;
		}
		if (m_camera == nullptr) {
			InitializeCamera();
		}
		std::cout << m_files.size() << std::endl;
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
		if (not m_snapshotted) {
			SaveSnapshot("snapshots/" + m_current_directory_filename + ".png", 600, 600);
		}
	}


	void TestMesh::SaveSnapshot(const std::string& filename, int width, int height) {
		// Allocate buffer to read pixels
		unsigned char* pixels = new unsigned char[width * height * 3];  // 3 channels (RGB)

		// Read pixels from the framebuffer
		glReadPixels(200, 200, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

		// Flip the image vertically (since OpenGL's origin is bottom-left)
		for (int i = 0; i < height / 2; ++i) {
			for (int j = 0; j < width * 3; ++j) {
				std::swap(pixels[i * width * 3 + j], pixels[(height - i - 1) * width * 3 + j]);
			}
		}

		// Use stb_image_write to save the image
		stbi_write_png(filename.c_str(), width, height, 3, pixels, width * 3);

		// Cleanup
		delete[] pixels;
		m_snapshotted = true;
		m_models.clear();
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
				std::filesystem::path pathObj(file);
				m_curModelObjectName = pathObj.stem().string();
				std::cout << "current model: " + m_curModelObjectName;
				m_models.clear();
				m_models.push_back(Model(m_curModelName));
				m_snapshotted = false;
			}
		}


	}



}
