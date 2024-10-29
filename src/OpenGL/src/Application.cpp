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


#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <normalization.cpp>

#include <tests/ShapeAnalyzer.cpp>

void analyzeFilesInDirectories(const std::string& basePath) {
    std::ofstream myfile;
    myfile.open("shapesDB.csv", std::ios::out | std::ios::app);
    myfile << "class; vertices;faces;triangles;quads;bmin; bmax\n";

    // Iterate through each directory inside the base path (e.g., Bus, Car, etc.)
    for (const auto& entry : fs::directory_iterator(basePath)) {
        if (fs::is_directory(entry.path())) {
            std::cout << "Analyzing category: " << entry.path().filename().string() << std::endl;

            // Iterate through each file in the directory
            for (const auto& file : fs::directory_iterator(entry.path())) {
                if (file.path().extension() == ".obj") {
                    ShapeAnalyzer analyzer;
                    std::cout << "Analyzing file: " << file.path() << std::endl;

                    // Call the analyze function for each OBJ file
                    analyzer.analyze(file.path().string(), myfile);

                }
            }


        }
    }
    myfile.close();
}


void normalizeFilesInDirectories(const std::string& basePath) {
    std::ofstream myfile;
    myfile.open("shapes_normalized_tightfit.csv", std::ios::out | std::ios::app);
    myfile << "class; scale_check ; X ; Y ; Z\n";

    std::ofstream myfile1;
    myfile1.open("shapes_normalized_dist.csv", std::ios::out | std::ios::app);
    myfile1 << "class; aligned_on_x ; aligned_on_y ; aligned_on_z ; flipped_on_x ; flipped_on_y ; flipped_on_z\n";
    for (const auto& entry : fs::directory_iterator(basePath)) {
        if (fs::is_directory(entry.path())) {
            std::cout << "Analyzing category: " << entry.path().filename().string() << std::endl;

            // Iterate through each file in the directory
            for (const auto& file : fs::directory_iterator(entry.path())) {
                if (file.path().extension() == ".obj") {
                    normalization normal;
                    std::cout << "NORMALIZING STEP!" << std::endl; 
                    auto [scale, box, flippedOn, alignedOn] = normal.normalizeShapeFile(file.path().string(), "output/" + entry.path().filename().string() + "/" + file.path().filename().string());
                    myfile << entry.path().filename().string() << ";" << scale << ";" << box[0] << ";" << box[1] << ";" << box[2] << std::endl;
                    //std::cout << "VERIFICATION STEP!" << std::endl;
                    //auto alignment = normal.alignmentAndFlipCheck("output/" + entry.path().filename().string() + "/" + file.path().filename().string());
                    //auto flippedOn = alignment[0];
                    //auto alignedOn = alignment[1];
                    myfile1 << entry.path().filename().string() << ";" << alignedOn[0] << ";" << alignedOn[1] << ";" << alignedOn[2] <<
                        ";" << flippedOn[0] << ";" << flippedOn[1] << ";" << flippedOn[2] << std::endl;
                }
            }
            

        }
    }
};

void verify(const std::string& basePath) {
    std::ofstream myfile;
    myfile.open("shapes_normalized_dist.csv", std::ios::out | std::ios::app);
    myfile << "class; obj_name ; aligned_on_x ; aligned_on_y ; aligned_on_z ; flipped_on_x ; flipped_on_y ; flipped_on_z\n";
    for (const auto& entry : fs::directory_iterator(basePath)) {
        if (fs::is_directory(entry.path())) {
            std::cout << "Analyzing category: " << entry.path().filename().string() << std::endl;

            // Iterate through each file in the directory
            for (const auto& file : fs::directory_iterator(entry.path())) {
                if (file.path().extension() == ".obj") {
                    normalization normal;
                    auto alignment = normal.alignmentAndFlipCheck(file.path().string());
                    auto flippedOn = alignment[0];
                    auto alignedOn = alignment[1];
                    myfile << entry.path().filename().string() << ";" << file.path().filename().string() << ";" << alignedOn[0] << ";" << alignedOn[1] << ";" << alignedOn[2] <<
                        ";" << flippedOn[0] << ";" << flippedOn[1] << ";" << flippedOn[2] << std::endl;
                }
            }



        }
    };
}





void save_snapshot(const std::string& basePath, GLFWwindow* window) {
    for (const auto& entry : fs::directory_iterator(basePath)) {
        if (fs::is_directory(entry.path())) {
            std::cout << "Analyzing category: " << entry.path().filename().string() << std::endl;

            // Iterate through each file in the directory
            for (const auto& file : fs::directory_iterator(entry.path())) {
                if (file.path().extension() == ".obj") {   
                    

                }
            }




        }
    };
}


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

        //std::string basePath = "C:/Users/Magnus/Documents/Master/MMR/MMR/src/OpenGL/test_output";
        //std::string basePath = "C:/Users/Magnus/Documents/Master/MMR/MMR/src/OpenGL/test_output";

        //std::string outputPath = "output";
        //normalizeFilesInDirectories(basePath);
        //add_bounding_box(outputPath);

        //verify(basePath);

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
        bool snapshotted = false;

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



//void verifyOriginInDirectories(const std::string& basePath) {
//    std::ofstream myfile;
//    myfile.open("shapes_normalized_dist.csv", std::ios::out | std::ios::app);
//    myfile << "class; dist_from_bary\n";
//    for (const auto& entry : fs::directory_iterator(basePath)) {
//        if (fs::is_directory(entry.path())) {
//            std::cout << "Analyzing category: " << entry.path().filename().string() << std::endl;
//
//            // Iterate through each file in the directory
//            for (const auto& file : fs::directory_iterator(entry.path())) {
//                if (file.path().extension() == ".obj") {
//                    normalization normal;
//                    float dist = normal.verify_origin(file.path().string());
//                    myfile << entry.path().filename().string() << ";" << dist << std::endl;
//                }
//            }
//            break;
//
//        }
//    }
//    myfile.close();
//}

