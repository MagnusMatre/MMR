#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <array>
#include <filesystem>

#include "Test.h"  // Include the Test base class
namespace fs = std::filesystem;
    class ShapeAnalyzer : public test::Test {
    public:
        ShapeAnalyzer() = default;

        void analyze(const std::string& objFile, std::ofstream& myfile) {
            std::ifstream file(objFile);
            if (!file.is_open()) {
                std::cerr << "Could not open the file: " << objFile << std::endl;
                return;
            }

            std::size_t lastSlash = objFile.find_last_of("/\\");  // find last occurrence of slash or backslash
            std::size_t secondLastSlash = objFile.find_last_of("/\\", lastSlash - 1); // find the second last occurrence of slash
            if (secondLastSlash != std::string::npos) {
                className = objFile.substr(secondLastSlash + 1, lastSlash - secondLastSlash - 1);
            }
            else {
                className = "Unknown"; // Fallback if class name extraction fails
            }

            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line); // if only v not vn
                std::string prefix;
                iss >> prefix;
                if (prefix == "v") {
                    // Handle vertex
                    float x, y, z;
                    iss >> x >> y >> z;
                    vertices.push_back({ x, y, z });
                    updateBoundingBox(x, y, z);
                }
                else if (prefix == "f") {
                    // Handle face
                    std::vector<int> face;
                    std::string vertexInfo;
                    while (iss >> vertexInfo) {
                        std::istringstream vertexStream(vertexInfo);
                        int vertexIndex;
                        vertexStream >> vertexIndex;
                        face.push_back(vertexIndex);
                    }
                    faces.push_back(face);
                    countFaceType(face);
                }
            }
            myfile << className << ";" << vertices.size() << ";" << faces.size() << ";" << triangleCount << ";" << quadCount << ";" <<
                "(" << minBoundingBox[0] << ", " << minBoundingBox[1] << ", " << minBoundingBox[2] << ")" << ";" <<
                "(" << maxBoundingBox[0] << ", " << maxBoundingBox[1] << ", " << maxBoundingBox[2] << ")" << "\n";
        }

    private:
        std::vector<std::array<float, 3>> vertices;
        std::vector<std::vector<int>> faces;
        std::array<float, 3> minBoundingBox;
        std::array<float, 3> maxBoundingBox;
        std::string className;
        int triangleCount = 0;
        int quadCount = 0;
        int otherCount = 0;

        void updateBoundingBox(float x, float y, float z) {
            if (x < minBoundingBox[0]) minBoundingBox[0] = x;
            if (y < minBoundingBox[1]) minBoundingBox[1] = y;
            if (z < minBoundingBox[2]) minBoundingBox[2] = z;

            if (x > maxBoundingBox[0]) maxBoundingBox[0] = x;
            if (y > maxBoundingBox[1]) maxBoundingBox[1] = y;
            if (z > maxBoundingBox[2]) maxBoundingBox[2] = z;
        }

        void countFaceType(const std::vector<int>& face) {
            if (face.size() == 3) {
                triangleCount++;
            }
            else if (face.size() == 4) {
                quadCount++;
            }
            else {
                otherCount++;
            }
        }
};
