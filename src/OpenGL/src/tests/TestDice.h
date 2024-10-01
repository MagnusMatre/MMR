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

namespace test {

	class TestDice : public Test
	{
	public:
		TestDice();
		~TestDice();

		void OnUpdate(GLFWwindow *window, float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		std::unique_ptr<Renderer> m_renderer;

		std::unique_ptr<IndexBuffer> m_ib;
		std::unique_ptr<VertexBuffer> m_vb;
		std::unique_ptr<VertexArray> m_va;
		std::unique_ptr<Shader> m_shader;
		std::unique_ptr<Texture> m_texture;
		
		glm::mat4 m_proj;
		glm::mat4 m_view;
		
		// Model matrix translations
		glm::vec3 m_translationA;
		glm::vec3 m_translationB;

		glm::mat4 m_mvp;

	};
}