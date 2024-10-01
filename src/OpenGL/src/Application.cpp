#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

#include "tests/TestClearColor.h"
#include "tests/TestDice.h"
#include "tests/TestCubes.h"
#include "tests/TestMesh.h"
#include "tests/TestRefineMesh.h"
#include "tests/TestClassMeshes.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
   
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 800, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // glfwSetCursorPosCallback(window, mouse_callback); // This works...

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cout << "Not ok" << std::endl;
    }
    else {
        std::cout << "Ok" << std::endl;
        std::cout << glGetString(GL_VERSION) << std::endl;
    }

    {
        Renderer renderer;

        GLCall(glEnable(GL_BLEND));
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        // Init ImGui context
        IMGUI_CHECKVERSION(); 
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true); 
        ImGui::StyleColorsDark();
        ImGui_ImplOpenGL3_Init((char*)glGetString(330));

        test::Test* currentTest = nullptr;
        test::TestMenu* testMenu = new test::TestMenu(currentTest);
        currentTest = testMenu;

        testMenu->RegisterTest<test::TestClearColor>("Clear Color");
        testMenu->RegisterTest<test::TestDice>("Dice texture");
        testMenu->RegisterTest<test::TestCubes>("Cubes");
        testMenu->RegisterTest<test::TestMesh>("Mesh");
        testMenu->RegisterTest<test::TestRefineMesh>("RefineMeshes");
        testMenu->RegisterTest<test::TestClassMeshes>("ClassifyMeshes");

        float deltaTime = 0.0f;	// time between current frame and last frame
        float lastFrame = 0.0f;

        bool initCallbacks = true;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
            GLCall(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
            renderer.Clear();

            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            //test.OnUpdate(0.0f);
            //test.OnRender();

            if (currentTest != testMenu && initCallbacks) {
                currentTest->SetWindow(window); // Preferable don't set callbacks every time but only on initialization...
                initCallbacks = false;
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (currentTest){
                currentTest->OnUpdate(window, deltaTime);
                currentTest->OnRender();

                ImGui::Begin("Test");
                if (currentTest != testMenu && ImGui::Button("<-")) {
                    initCallbacks = true;
                    delete currentTest;
                    currentTest = testMenu;
                }
                currentTest->OnImGuiRender();

                ImGui::End();
                
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

    }
    ImGui_ImplOpenGL3_Shutdown(); 
    ImGui_ImplGlfw_Shutdown(); 
    ImGui::DestroyContext(); 
    glfwTerminate();

    return 0;
};