#include "TestRefineMesh.h"

#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Renderer.h"

#include <iostream>
#include <filesystem>


namespace test {

	TestRefineMesh::TestRefineMesh()
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

		// Load first mesh and initialize directory variables
		m_refinedMeshPathOutput = "../../data/RefinedMeshes";
		//m_refinedMeshPathOutput = "C:/Users/timgr/Documents/GitHub/MMR/data/RefinedMeshes/AircraftBuoyant/name";
		m_dataRoot = "../../data";
		m_curDirectory = m_dataRoot + "/Meshes";
		loadCurrentDirectory(); 

		m_renderer = std::make_unique<Renderer>();

		m_shaderMeshSolid = std::make_unique<Shader>("resources/shaders/MeshSolid.shader");
		m_shaderMeshWireframe = std::make_unique<Shader>("resources/shaders/MeshWireframe.shader");
		m_shaderDot = std::make_unique<Shader>("resources/shaders/Dot.shader");	
	}

	TestRefineMesh::~TestRefineMesh() {

	}

	void TestRefineMesh::SetWindow(GLFWwindow* window) {
		m_window = window;
	}

	void TestRefineMesh::InitializeCamera() {
		m_camera = std::make_unique<Camera>(m_window, glm::vec3(0.0f, 0.0f, 5.0f));
	}

	void TestRefineMesh::OnUpdate(GLFWwindow* window, float deltaTime) {
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

	void TestRefineMesh::OnRender() {

		if (m_curGeomMesh == nullptr) {
			return;
		}


		glm::mat4 modelMesh = glm::translate(glm::mat4(1.0f), m_translationMesh);
		m_mvp = m_projection * m_view * modelMesh;

		if (m_testStructure) {
			//highlightSelectedVertex(modelMesh, m_mvp);
		}

		if (m_renderMode == RenderMode::OnlySolid) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			m_shaderMeshSolid->Bind();
			m_shaderMeshSolid->SetUniformMat4f("u_MVP", m_mvp);
			m_shaderMeshSolid->SetUniformMat4f("u_model", modelMesh);
			m_shaderMeshSolid->SetUniform3f("u_lightcolor", m_lightcolor.x, m_lightcolor.y, m_lightcolor.z);
			m_shaderMeshSolid->SetUniform3f("u_ambientlightcolor", m_ambientlightcolor.x, m_ambientlightcolor.y, m_ambientlightcolor.z);
			m_shaderMeshSolid->SetUniform3f("u_lightpos", m_translationLight.x, m_translationLight.y, m_translationLight.z);
			m_shaderMeshSolid->SetUniform3f("u_viewpos", m_camera->Position.x, m_camera->Position.y, m_camera->Position.z);
			//m_model->Draw(*m_shaderMeshSolid, *m_renderer);
			m_curGeomMesh->DrawMesh(*m_shaderMeshSolid, *m_renderer);
			m_shaderMeshSolid->Unbind();
		}
		else if (m_renderMode == RenderMode::OnlyWireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			m_shaderMeshWireframe->Bind();
			m_shaderMeshWireframe->SetUniformMat4f("u_MVP", m_mvp);
			m_shaderMeshWireframe->SetUniform3f("u_wireframecolor", m_wireFrameColor.x, m_wireFrameColor.y, m_wireFrameColor.z);
			m_shaderMeshWireframe->SetUniform1f("u_wireframewidth", m_wireFrameWidth);
			//m_model->Draw(*m_shaderMeshWireframe, *m_renderer);
			m_curGeomMesh->DrawMesh(*m_shaderMeshWireframe, *m_renderer);
			m_shaderMeshWireframe->Unbind();
		}
		else if (m_renderMode == RenderMode::WireframeAndSolid) {
			glEnable(GL_POLYGON_OFFSET_LINE);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(2.0f, 2.0f);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			m_shaderMeshSolid->Bind();
			m_shaderMeshSolid->SetUniformMat4f("u_MVP", m_mvp);
			m_shaderMeshSolid->SetUniformMat4f("u_model", modelMesh);
			m_shaderMeshSolid->SetUniform3f("u_lightcolor", m_lightcolor.x, m_lightcolor.y, m_lightcolor.z);
			m_shaderMeshSolid->SetUniform3f("u_ambientlightcolor", m_ambientlightcolor.x, m_ambientlightcolor.y, m_ambientlightcolor.z);
			m_shaderMeshSolid->SetUniform3f("u_lightpos", m_translationLight.x, m_translationLight.y, m_translationLight.z);
			m_shaderMeshSolid->SetUniform3f("u_viewpos", m_camera->Position.x, m_camera->Position.y, m_camera->Position.z);
			//m_model->Draw(*m_shaderMeshSolid, *m_renderer);
			m_curGeomMesh->DrawMesh(*m_shaderMeshSolid, *m_renderer);
			m_shaderMeshSolid->Unbind();

			glPolygonOffset(1.0f, 1.0f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			m_shaderMeshWireframe->Bind();
			m_shaderMeshWireframe->SetUniformMat4f("u_MVP", m_mvp);
			m_shaderMeshWireframe->SetUniform3f("u_wireframecolor", m_wireFrameColor.x, m_wireFrameColor.y, m_wireFrameColor.z);
			m_shaderMeshWireframe->SetUniform1f("u_wireframewidth", m_wireFrameWidth);
			//m_model->Draw(*m_shaderMeshWireframe, *m_renderer);
			m_curGeomMesh->DrawMesh(*m_shaderMeshWireframe, *m_renderer);
			m_shaderMeshWireframe->Unbind();
		}
	}

	void TestRefineMesh::loadNewMesh(const std::string& path) {
		m_curModelName = path;
		m_model = new Model(m_curModelName);
		m_curGeomMesh = std::make_unique<GeomMesh>(m_curModelName, false);
		m_curGeomMesh->PrintStats();
	}

	void TestRefineMesh::loadCurrentDirectory() {
		m_curFiles.clear();
		m_curDirectories.clear();
		m_model = nullptr;
		m_curModelName = "";
		m_curGeomMesh = nullptr;

		if (std::filesystem::exists(m_curDirectory)) {
			for (const auto& entry : std::filesystem::directory_iterator(m_curDirectory)) {
				if (entry.is_directory()) {
					std::cout << "Path directory: " << entry.path().string() << std::endl;
					m_curDirectories.push_back(entry.path().string());
				}
				else if (entry.is_regular_file() && entry.path().extension() == ".obj") {
					std::cout << "Path file: " << entry.path().string() << std::endl;
					if (m_curModelName.empty()) {
						InitializeCamera();
						loadNewMesh(entry.path().string());
					}
					m_curFiles.push_back(entry.path().string());
				}
			}
		}
		else {
			std::cerr << "Directory not found: " << m_curDirectory << std::endl;
		}
	}

    void TestRefineMesh::OnImGuiRender() {
        ImGui::SliderFloat3("Translation light source", &m_translationLight.x, -5.0f, 5.0f);
        ImGui::SliderFloat3("Translation mesh", &m_translationMesh.x, -5.0f, 5.0f);
        ImGui::SliderFloat3("Translation camera", &m_translationCamera.x, -5.0, 5.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::Text("Camera:");
        ImGui::Text("X.pos: %.3f, Y.pos: %.3f, Z.pos: %.3f ", m_camera->Position.x, m_camera->Position.y, m_camera->Position.z);
		ImGui::Text("Yaw: %.3f, Pitch: %.3f", m_camera->Yaw, m_camera->Pitch);

        if (ImGui::Button("RESET CAMERA")) {
            m_camera->Position = glm::vec3(0.0f, 0.0f, 5.0f);
            m_camera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
            m_camera->Yaw = -90.0f;
            m_camera->Pitch = 0.0f;
        }

		// Box to enable testing of the data structure
		ImGui::Checkbox("Test data structure", &m_testStructure);

		// Button for selecting random vertex of mesh
		if (m_curGeomMesh != nullptr && m_curGeomMesh->m_meshVertices.size() > 0) {
			if (ImGui::Button("Select Random Vertex")) {
				m_curGeomMesh->SelectRandomVertex();
			}
		}

		// Slider for changing the size of the dot
		ImGui::SliderFloat("Dot size", &m_dotSize, 3.0f, 15.0f);

		// Button for refining the mesh
		if (m_curModelName != "" && ImGui::Button("Refine Mesh")) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Set button color to red
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f)); // Center-align button text
            ImGui::Button("Refine Mesh", ImVec2(200, 50)); // Increase button size to 200x50

            ImGui::PopStyleVar(); // Reset button text alignment
            ImGui::PopStyleColor(); // Reset button color
			m_curGeomMesh->SimpleRefineLoop();
			m_curGeomMesh->PrintStats();
		}

		// Button for saving the mesh
		if (m_curModelName != "" && ImGui::Button("Save Mesh")) {
			std::filesystem::path filePath(m_curModelName);
			std::string fileName = filePath.filename().string();
			std::string directoryName = filePath.parent_path().filename().string();

			std::cout << m_refinedMeshPathOutput << " " << fileName << " " << directoryName << std::endl;

			std::string outputFile = m_refinedMeshPathOutput + "/" + directoryName + "/" + fileName;
			m_curGeomMesh->SaveMesh(outputFile);
		}


        // Button for going up one directory
        std::string textToDisplay;
        textToDisplay = "Current directory: " + m_curDirectory;
        ImGui::Text(textToDisplay.c_str());
        if (m_curDirectory != m_dataRoot && ImGui::Button("<--##label1")) {
            std::filesystem::path parentDirectory = std::filesystem::path(m_curDirectory).parent_path();
            m_curDirectory = parentDirectory.string();
            loadCurrentDirectory();
        }

        textToDisplay = "Choose model: (current: " + m_curModelName + ")";
        ImGui::Text(textToDisplay.c_str());
        for (auto& file : m_curFiles) {
			std::string buttonLabel = file + "##label4";
            if (ImGui::Button(buttonLabel.c_str())) {
				InitializeCamera();
				loadNewMesh(file);
            }
        }

        // Buttons for going into a subdirectory if there are any
        for (auto& directory : m_curDirectories) {
            std::string buttonLabel = directory + "##label5";
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.6f, 1.0f)); // Set button color

            if (ImGui::Button(buttonLabel.c_str())) {
                m_curDirectory = directory;
                loadCurrentDirectory();
            }

            ImGui::PopStyleColor(); // Reset button colors
        }
    }

	void TestRefineMesh::highlightSelectedVertex(glm::mat4 model, glm::mat4 mvp) {
		// Highlight the selected vertex
		if (m_curGeomMesh != nullptr && m_curGeomMesh->m_meshVertices.size() > 0) {
			// Get the selected vertex
			int selectedVertex = m_curGeomMesh->m_selectedVertex;
			if (selectedVertex >= 0 && selectedVertex < m_curGeomMesh->m_meshVertices.size()) {
				// Get the position of the selected vertex
				glm::vec3 selectedVertexPos = m_curGeomMesh->m_meshVertices[selectedVertex].position;

				// Draw a dot at the position of the selected vertex
				//m_renderer->DrawDot(selectedVertexPos, m_dotSize, glm::vec3(1.0f, 0.0f, 0.0f), mvp);

				// Draw the adjacent vertices
				m_shaderDot->Bind();
				m_shaderDot->SetUniformMat4f("u_MVP", mvp);
				m_shaderDot->SetUniform4f("u_color", 0.0f, 1.0f, 0.0f, 1.0f);
				m_renderer->Draw(*m_curGeomMesh->m_incidentVerticesVAO, *m_curGeomMesh->m_incidentVerticesIBO, *m_shaderDot, GL_POINTS);
				m_shaderDot->Unbind();

				// Draw the incident triangles (causes crash)
				/*glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(1.0f, 1.0f);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				m_shaderDot->Bind();
				m_shaderDot->SetUniformMat4f("u_MVP", mvp);
				m_shaderDot->SetUniform4f("u_color", 1.0f, 1.0f, 0.0f, 1.0f);
				m_renderer->Draw(*m_curGeomMesh->m_incidentTrianglesVAO, *m_curGeomMesh->m_incidentTrianglesIBO, *m_shaderDot, GL_TRIANGLES);
				m_shaderDot->Unbind();*/

				// Draw the incident edges (+ extra triangle edges)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glEnable(GL_POLYGON_OFFSET_LINE);
				glPolygonOffset(-1.0f, -1.0f);
				m_shaderMeshWireframe->Bind();
				m_shaderMeshWireframe->SetUniformMat4f("u_MVP", mvp);
				m_shaderMeshWireframe->SetUniform3f("u_wireframecolor", m_edgeColor.x, m_edgeColor.y, m_edgeColor.z);
				m_shaderMeshWireframe->SetUniform1f("u_wireframewidth", m_wireFrameWidth);
				m_renderer->Draw(*m_curGeomMesh->m_incidentTrianglesVAO, *m_curGeomMesh->m_incidentTrianglesIBO, *m_shaderMeshWireframe, GL_TRIANGLES);
				m_shaderMeshWireframe->Unbind();
				glDisable(GL_POLYGON_OFFSET_LINE);
				
			}
		}

	}

}
