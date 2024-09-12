#include "TestCubes.h"

#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Renderer.h"

#include <iostream>


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

	TestCubes::TestCubes()
		: m_cubecolor(glm::vec3(0.3f, 0.1f, 0.9f)), m_lightcolor(glm::vec3(1.0f, 1.0f, 1.0f)),
		m_translationLight(0.0f, 0.0f, 0.0f), m_translationCube(0.0f, 0.0f, 0.0f),
		m_projection(glm::perspective(glm::radians(45.0f), (float)800 / (float)800, 0.1f, 100.0f))
	{

		//glfwSetCursorPosCallback(window, on_mouse_callback); // Could use static functions for callbacks...
		//glfwSetScrollCallback(window, on_zoom_callback);

		GLCall(glEnable(GL_BLEND));
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		//float positions[24] = { // Specify vertices of a cube in 3d space with grid 960x540
		//	-100.0f, -100.0f, -100.0f, // 0
		//	100.0f, -100.0f, -100.0f,  // 1
		//		-100.0f, 100.0f, -100.0f,  // 2
		//	100.0f, 100.0f, -100.0f,   // 3
		//	-100.0f, -100.0f, 100.0f,  // 4
		//	100.0f, -100.0f, 100.0f,   // 5
		//	-100.0f, 100.0f, 100.0f,   // 6
		//	100.0f, 100.0f, 100.0f     // 7
		//};
		/*float positions[] = {
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f
							   
			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,
							   
			-0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f, 
							   
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f, 
			0.5f, 0.5f, 0.5f,
							   
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f, 
			0.5f, -0.5f, 0.5f, 
			-0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,
							   
			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f,
		};*/
		// vertices required to draw a cube.
		float positions[] = {	1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,   // v0,v1,v2,v3 (front)
								1, 1, 1,   1,-1, 1,   1,-1,-1,   1, 1,-1,   // v0,v3,v4,v5 (right)
								1, 1, 1,   1, 1,-1,  -1, 1,-1,  -1, 1, 1,   // v0,v5,v6,v1 (top)
							   -1, 1, 1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1,   // v1,v6,v7,v2 (left)
							   -1,-1,-1,   1,-1,-1,   1,-1, 1,  -1,-1, 1,   // v7,v4,v3,v2 (bottom)
								1,-1,-1,  -1,-1,-1,  -1, 1,-1,   1, 1,-1 }; // v4,v7,v6,v5 (back)

		// normal array
		float normals[] = {		0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,   // v0,v1,v2,v3 (front)
								1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,   // v0,v3,v4,v5 (right)
								0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,   // v0,v5,v6,v1 (top)
							   -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,   // v1,v6,v7,v2 (left)
								0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0,   // v7,v4,v3,v2 (bottom)
								0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1 }; // v4,v7,v6,v5 (back)

		// color array
		float colors[] ={		1, 1, 1,   1, 1, 0,   1, 0, 0,   1, 0, 1,   // v0,v1,v2,v3 (front)
								1, 1, 1,   1, 0, 1,   0, 0, 1,   0, 1, 1,   // v0,v3,v4,v5 (right)
								1, 1, 1,   0, 1, 1,   0, 1, 0,   1, 1, 0,   // v0,v5,v6,v1 (top)
								1, 1, 0,   0, 1, 0,   0, 0, 0,   1, 0, 0,   // v1,v6,v7,v2 (left)
								0, 0, 0,   0, 0, 1,   1, 0, 1,   1, 0, 0,   // v7,v4,v3,v2 (bottom)
								0, 0, 1,   0, 0, 0,   0, 1, 0,   0, 1, 1 }; // v4,v7,v6,v5 (back)


		unsigned int indices[] = { 0, 1, 2,   2, 3, 0,      // front
								   4, 5, 6,   6, 7, 4,      // right
								   8, 9,10,  10,11, 8,      // top
								  12,13,14,  14,15,12,      // left
								  16,17,18,  18,19,16,      // bottom
								  20,21,22,  22,23,20 };    // back

		//unsigned int indices[36] = { // Cube consists of 2 * 6 triangles = 36 indices
		//	0, 1, 2,
		//	1, 3, 2,
		//	1, 3, 5,
		//	3, 7, 5,
		//	3, 7, 6,
		//	3, 6, 2,
		//	2, 6, 4, 
		//	2, 4, 0,
		//	0, 1, 5,
		//	0, 5, 4,
		//	4, 5, 7,
		//	4, 7, 6
		//};

		//unsigned int indices[] = {
		//	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35
		//};

		m_renderer = std::make_unique<Renderer>();

		m_va = std::make_unique<VertexArray>();
		m_vb = std::make_unique<VertexBuffer>(positions, 36 * 3 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		m_va->AddBuffer(*m_vb, layout);

		m_ib = std::make_unique<IndexBuffer>(indices, 36);

		m_shaderCube = std::make_unique<Shader>("resources/shaders/Lighting.shader");
		m_shaderCube->Bind();

		m_shaderCube->Unbind();

		m_shaderLight = std::make_unique<Shader>("resources/shaders/LightSource.shader");
		m_shaderLight->Bind();

		m_shaderLight->Unbind();

		m_ib->Unbind();
	}

	TestCubes::~TestCubes() {

	}

	void TestCubes::InitializeCamera(GLFWwindow* window) {
		m_camera = std::make_unique<Camera>(window);
	}

	void TestCubes::OnUpdate(GLFWwindow *window, float deltaTime) {
		m_camera->ProcessKeyboard(deltaTime);
		m_view = m_camera->GetViewMatrix();
		//if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		//	glfwSetWindowShouldClose(window, true);

		/*Camera_Movement direction;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
			direction = FORWARD;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
			direction = BACKWARD;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
			direction = LEFT;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
			direction = RIGHT;

		m_camera->ProcessKeyboard(direction, deltaTime);*/



		//m_camera->ProcessMouseMovement();
	}

	void TestCubes::OnRender() {

		//m_view = glm::translate(glm::mat4(1.0f), -m_translationCamera

		{ // Draw light source cube

		    glm::mat4 modelA = glm::translate(glm::mat4(1.0f), m_translationLight);
		    m_mvp = m_projection * m_view * modelA;
			m_shaderLight->Bind();
		    m_shaderLight->SetUniformMat4f("u_MVP", m_mvp);
			m_renderer->Draw(*m_va, *m_ib, *m_shaderLight);
			//m_renderer->Draw(*m_va, *m_shaderLight);
		}
		{ // Draw other cube
			glm::mat4 modelB = glm::translate(glm::mat4(1.0f), m_translationCube);
		    m_mvp = m_projection * m_view * modelB; 
			m_shaderCube->Bind();
		    m_shaderCube->SetUniformMat4f("u_MVP", m_mvp); 
			m_shaderCube->SetUniform3f("u_objectcolor", m_cubecolor.x, m_cubecolor.y, m_cubecolor.z);
			m_shaderCube->SetUniform3f("u_lightcolor", m_lightcolor.x, m_lightcolor.y, m_lightcolor.z);
			m_renderer->Draw(*m_va, *m_ib, *m_shaderCube);
			//m_renderer->Draw(*m_va, *m_shaderLight);
		}

	}

	void TestCubes::OnImGuiRender() {
		ImGui::SliderFloat3("Translation A", &m_translationLight.x, -5.0f, 5.0f);            
		ImGui::SliderFloat3("Translation B", &m_translationCube.x, -5.0f, 5.0f);
		ImGui::SliderFloat3("Translation Camera", &m_translationCamera.x, -5.0, 5.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Text("Camera:");
		ImGui::Text("X.pos: %.3f, Y.pos: %.3f, Z.pos: %.3f ", m_camera->Position.x, m_camera->Position.y, m_camera->Position.z);
		if (ImGui::Button("RESET CAMERA")) {
			m_camera->Position = glm::vec3(0.0f, 0.0f, 0.0f);
			m_camera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
			m_camera->Yaw = -90.0f;
			m_camera->Pitch = 0.0f;
		}

	}

}
