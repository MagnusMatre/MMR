#include "TestDice.h"

#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Renderer.h"

#include <memory>

namespace test {

	TestDice::TestDice()
		: m_translationA(0, 0, 0), m_translationB(0, 0, 0),
		  m_proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)), 
		  m_view(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)))
	{

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		float positions[16] = {
			-50.0f, -50.0f, 0.0f, 0.0f,
			50.0f,  -50.0f, 1.0f, 0.0f,
			50.0f, 50.0f, 1.0f, 1.0f,
			-50.0f, 50.0f, 0.0f, 1.0f,
		};

		unsigned int indices[6] = {
			0, 1, 2,
			2, 3, 0
		};

		m_renderer = std::make_unique<Renderer>();

		m_va = std::make_unique<VertexArray>();
		m_vb = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		m_va->AddBuffer(*m_vb, layout);

		m_ib = std::make_unique<IndexBuffer>(indices, 6);

		m_shader = std::make_unique<Shader>("resources/shaders/Basic.shader");
		m_shader->Bind();
		m_texture = std::make_unique<Texture>("resources/textures/dice.png");
		m_shader->SetUniform1i("u_Texture", 0);

		m_shader->Unbind();
		m_ib->Unbind();
	}

	TestDice::~TestDice() {

	}

	void TestDice::OnUpdate(GLFWwindow* window, float deltaTime) {

	}

	void TestDice::OnRender() {
		//GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

		//Renderer renderer;

		m_texture->Bind();

		{
		    glm::mat4 modelA = glm::translate(glm::mat4(1.0f), m_translationA);
		    m_mvp = m_proj * m_view * modelA;
			m_shader->Bind();
		    m_shader->SetUniformMat4f("u_MVP", m_mvp);
		    //renderer.Draw(*m_va, *m_ib, *m_shader);
			m_renderer->Draw(*m_va, *m_ib, *m_shader, GL_TRIANGLES);
		}
		{
			glm::mat4 modelB = glm::translate(glm::mat4(1.0f), m_translationB);
		    m_mvp = m_proj * m_view * modelB; 
			m_shader->Bind();
		    m_shader->SetUniformMat4f("u_MVP", m_mvp); 
		    //renderer.Draw(*m_va, *m_ib, *m_shader); 
			m_renderer->Draw(*m_va, *m_ib, *m_shader, GL_TRIANGLES);
		}

	}

	void TestDice::OnImGuiRender() {
		ImGui::SliderFloat3("Translation A", &m_translationA.x, 0.0f, 960.0f);            
		ImGui::SliderFloat3("Translation B", &m_translationB.x, 0.0f, 960.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}