#include "TestFeatures.h"

#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>

#include "Renderer.h"

#include <iostream>
#include <filesystem>


namespace test {

	TestFeatures::TestFeatures()
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

		m_featureFile = "../../res/features_okaymeshes/features_clean.txt";
		std::string saveDistanceMatrix = "../../res/features_okaymeshes/distance_matrix.txt";

		m_queryEngine.LoadFeatures(m_featureFile);
		std::cout << "Features loaded" << std::endl;
		//m_queryEngine.ComputeFullDistanceMatrix();
		//std::cout << "Distances computed" << std::endl;
		//m_queryEngine.SaveDistanceMatrix(saveDistanceMatrix);

		m_dataRoot = "../../data";
		m_curDirectory = m_dataRoot + "/OkayMeshes";
		m_samplePointsDirectory = "../../res/sample_points";
		loadCurrentDirectory(); 

		m_renderer = std::make_unique<Renderer>();

		m_shaderMeshSolid = std::make_unique<Shader>("resources/shaders/MeshSolid.shader");
		m_shaderMeshWireframe = std::make_unique<Shader>("resources/shaders/MeshWireframe.shader");
		m_shaderDot = std::make_unique<Shader>("resources/shaders/Dot.shader");	
	}

	TestFeatures::~TestFeatures() {

	}

	void TestFeatures::SetWindow(GLFWwindow* window) {
		m_window = window;
	}

	void TestFeatures::InitializeCamera() {
		m_camera = std::make_unique<Camera>(m_window, glm::vec3(0.0f, 0.0f, 5.0f));
	}

	void TestFeatures::OnUpdate(GLFWwindow* window, float deltaTime) {
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

	void TestFeatures::OnRender() {

		if (m_curGeomMesh == nullptr) {
			return;
		}


		glm::mat4 modelMesh = glm::translate(glm::mat4(1.0f), m_translationMesh);
		m_mvp = m_projection * m_view * modelMesh;

		// If the random points vector is not empty from FeatureExtraction, draw the points
		/*if (!m_featureExtractor.m_random_vertices.empty()) {
			glEnable(GL_POLYGON_OFFSET_POINT);
			glPolygonOffset(-1.0f, -1.0f);
			m_shaderDot->Bind();
			m_shaderDot->SetUniformMat4f("u_MVP", m_mvp);
			m_shaderDot->SetUniform4f("u_color", 1.0f, 0.0f, 0.0f, 1.0f);
			m_renderer->Draw(*m_sampledVerticesVAO, *m_sampledVerticesIBO, *m_shaderDot, GL_POINTS);
			m_shaderDot->Unbind();
		}*/

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

	void TestFeatures::loadNewMesh(const std::string& path) {
		m_curModelName = path;
		m_model = std::make_unique<Model>(m_curModelName);
		m_curGeomMesh = std::make_unique<GeomMesh>(m_curModelName, false);
		m_curGeomMesh->PrintStats();

		//loadCGALmesh(m_curModelName);

		computeDistances();
		

		//Compute the distance from the current model to all other models in the same directory
		/*std::string class_name = std::filesystem::path(m_curModelName).parent_path().filename().string();
		std::string feature_file = m_dataRoot + "/features_okaymeshes/features_clean.txt";
		m_queryEngine.LoadFeatures(feature_file);*/
		//for (auto& file : m_curFiles) {
		//	if (file != m_curModelName) {
		//		// Extract directory and filename from the path
		//		std::filesystem::path p(file);
		//		std::string class_name_other = p.parent_path().filename().string();
		//		std::string file_name_other = p.filename().string();
		//		std::string other_obj = class_name_other + "/" + file_name_other;

		//		std::filesystem::path p2(m_curModelName);
		//		std::string class_name_query = p2.parent_path().filename().string();
		//		std::string file_name_query = p2.filename().string();
		//		std::string query_obj = class_name_query + "/" + file_name_query;

		//		std::cout << query_obj << " " << other_obj << std::endl;

		//		m_queryEngine.ComputeDistance(query_obj, other_obj);


		//	}
		//}

		/*try {
			loadSampleVertices();
		}
		catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}*/
	}

	void TestFeatures::loadCurrentDirectory() {
		m_curFiles.clear();
		m_curDirectories.clear();
		m_model = nullptr;
		m_curModelName = "";
		m_curGeomMesh = nullptr;

		if (std::filesystem::exists(m_curDirectory)) {
			for (const auto& entry : std::filesystem::directory_iterator(m_curDirectory)) {
				if (entry.is_directory()) {
					//std::cout << "Path directory: " << entry.path().string() << std::endl;
					m_curDirectories.push_back(entry.path().string());
				}
				else if (entry.is_regular_file() && entry.path().extension() == ".obj") {
					//std::cout << "Path file: " << entry.path().string() << std::endl;
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

    void TestFeatures::OnImGuiRender() {
        //ImGui::SliderFloat3("Translation light source", &m_translationLight.x, -5.0f, 5.0f);
        //ImGui::SliderFloat3("Translation mesh", &m_translationMesh.x, -5.0f, 5.0f);
        //ImGui::SliderFloat3("Translation camera", &m_translationCamera.x, -5.0, 5.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        //ImGui::Text("Camera:");
        //ImGui::Text("X.pos: %.3f, Y.pos: %.3f, Z.pos: %.3f ", m_camera->Position.x, m_camera->Position.y, m_camera->Position.z);
		//ImGui::Text("Yaw: %.3f, Pitch: %.3f", m_camera->Yaw, m_camera->Pitch);

        /*if (ImGui::Button("RESET CAMERA")) {
            m_camera->Position = glm::vec3(0.0f, 0.0f, 5.0f);
            m_camera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
            m_camera->Yaw = -90.0f;
            m_camera->Pitch = 0.0f;
        }*/

		// Button for recomputing the distances
		if (ImGui::Button("Recompute distances")) {
			computeDistances();
		}

		// Create sliders for each of the SCALAR_FEATURES to change the weights
		for (int i = 0; i < NUM_SCALAR_FEATURES; i++) {
			std::string label = "Scalar weight " + m_queryEngine.m_scalar_feature_names[i];
			ImGui::SliderFloat(label.c_str(), &m_queryEngine.m_scalar_weights[i], 0.0f, 5.0f);
		}

		for (int i = 0; i < NUM_HISTOGRAM_FEATURES; i++) {
			// Change color of button to red
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.6f, 1.0f)); // Set button color

			std::string label = "Histogram weight " + m_queryEngine.m_histogram_feature_names[i];
			ImGui::SliderFloat(label.c_str(), &m_queryEngine.m_histogram_weights[i], 0.0f, 5.0f);

			ImGui::PopStyleColor(); // Reset button colors
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

	void TestFeatures::updateSampleVerticesBuffer() {
		std::vector<float> vertices;
		std::vector<unsigned int> indices;

		// Find the vertices and indices to create the triangles
		int index = 0;
		for (Point_3 p : m_featureExtractor.m_random_vertices) {
			// Add the vertices to the list
			vertices.push_back((float)p[0]);
			vertices.push_back((float)p[1]);
			vertices.push_back((float)p[2]);

			indices.push_back(index++);
		}

		// POINTS
		// Create the vertex array
		m_sampledVerticesVAO = std::make_unique<VertexArray>();
		m_sampledVerticesVBO = std::make_unique<VertexBuffer>(&vertices[0], sizeof(float) * vertices.size());
		VertexBufferLayout layout;
		layout.Push<float>(3);
		m_sampledVerticesVAO->AddBuffer(*m_sampledVerticesVBO, layout);

		// Create the index buffer
		m_sampledVerticesIBO = std::make_unique<IndexBuffer>(&indices[0], indices.size());
		m_sampledVerticesIBO->Unbind();
	}

	void TestFeatures::loadCGALmesh(std::string& filename) {
		m_CGALmesh.clear();
		if (!CGALPMP::IO::read_polygon_mesh(filename, m_CGALmesh)) {
			std::cerr << "Error: cannot read file " << filename << std::endl;
			exit(EXIT_FAILURE);
		}

		if (CGAL::is_empty(m_CGALmesh))
		{
			std::cerr << "Warning: empty file?" << std::endl;
			return;
		}

		std::cout << "Mesh successfully loaded CGAL mesh" << std::endl;

		//m_featureExtractor.get_N_random_vertices(m_CGALmesh, 10000);
		//updateSampleVerticesBuffer();
	}

	void TestFeatures::loadSampleVertices() {
		// Get class name and file name from m_curModelName
		std::filesystem::path p(m_curModelName);
		std::string class_name = p.parent_path().filename().string();
		std::string file_name = p.filename().string();

		// Replace .obj with .txt to obtain the true file name
		file_name.replace(file_name.end() - 4, file_name.end(), ".txt");

		// Load the .txt file corresponding to file_name
		std::string sample_points_file = m_samplePointsDirectory + "/" + class_name + "/" + file_name;
		std::ifstream file(sample_points_file);
		if (!file.is_open()) {
			std::cerr << "Error: cannot open file " << sample_points_file << std::endl;
			return;
		} 

		std::vector<Point_3> sample_points;
		std::string line;
		while (std::getline(file, line)) {
			std::istringstream iss(line);
			double x, y, z;
			if (!(iss >> x >> y >> z)) {
				std::cerr << "Error: cannot read line " << line << std::endl;
				break;
			}
			m_featureExtractor.m_random_vertices.push_back(Point_3(x, y, z));
		}

		updateSampleVerticesBuffer();
	}

	void TestFeatures::computeDistances() {
		std::filesystem::path p(m_curModelName);
		std::string class_name = p.parent_path().filename().string();
		std::string obj_name = p.filename().string();
		std::string file_name = class_name + "/" + obj_name;

		// Retrieve the distances to all other objects
		int modelIndex = m_queryEngine.getIndex(file_name);
		std::vector<std::pair<float, int>> distances;
		for (int i = 0; i < NUM_SHAPES; i++) {
			if (i != modelIndex) {
				float distance = m_queryEngine.ComputeDistance(file_name, m_queryEngine.m_name_map[i]);
				distances.push_back(std::make_pair(distance, i));
			}
		}

		// Sort the distances
		std::sort(distances.begin(), distances.end());

		// Print the 20 closest
		for (int i = 0; i < 20; i++) {
			std::cout << i + 1 << " - Distance to " << m_queryEngine.m_name_map[distances[i].second] << " is: " << distances[i].first << std::endl;
		}
	}
}

