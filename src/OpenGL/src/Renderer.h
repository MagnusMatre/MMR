#pragma once

#include <GL/glew.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))


void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer {
public:
    Renderer();
    ~Renderer();

    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, unsigned int renderType);
    void Draw(const VertexArray& va, const Shader& shader);
	void DrawDot(glm::vec3 position, float radius, glm::vec3 color, glm::mat4 mvp);
    void Clear();
};
 