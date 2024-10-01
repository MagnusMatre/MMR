#include "Renderer.h"
#include <iostream>

#include "VertexBufferLayout.h"

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "GL error: (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

Renderer::Renderer() {

}

Renderer::~Renderer() {

}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, unsigned int renderType) {
    shader.Bind();
    va.Bind();
    ib.Bind();

    GLCall(glDrawElements(renderType, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(const VertexArray& va, const Shader& shader) {
    shader.Bind();
    va.Bind();
    GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
}

void Renderer::DrawDot(glm::vec3 position, float size, glm::vec3 color, glm::mat4 mvp) {
    // Draw a dot on the screen
    // Create a vertex array
    GLCall(glPointSize(size));

    float vertices[] = {
        // positions
        position.x, position.y, position.z
    };

    VertexArray va;
    VertexBuffer vb(vertices, 3 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    va.AddBuffer(vb, layout);

    // Create an index buffer
    unsigned int indices[] = {
        0
    };

    IndexBuffer ib(indices, 1);

    // Create a shader
    Shader shader("resources/shaders/Dot.shader");
    shader.Bind();
    shader.SetUniformMat4f("u_MVP", mvp);
    shader.SetUniform4f("u_color", color.x, color.y, color.z, 1.0f);
    //shader.SetUniform1f("u_size", size); // Set the size uniform

    va.Bind();
    ib.Bind();

    GLCall(glDrawElements(GL_POINTS, ib.GetCount(), GL_UNSIGNED_INT, nullptr));

    va.Unbind();
    ib.Unbind();
    shader.Unbind();
}

void Renderer::Clear() {
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}
