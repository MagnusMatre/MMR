
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <string>
#include <Eigen/Dense> 
#include <array>

class normalization {
public:

float findMaxAbsoluteValue(const std::pair<std::array<float, 3>, std::array<float, 3>>& list) {
    float maxAbsValue = 0.0f;
    const auto& arr1 = list.first;
    const auto& arr2 = list.second;

    for (float value : arr1) {
        maxAbsValue = std::max(maxAbsValue, std::abs(value));
    }

    for (float value : arr2) {
        maxAbsValue = std::max(maxAbsValue, std::abs(value));
    }
    

    return maxAbsValue;
}

std::array<float, 3> computeBarycenter(const std::vector<std::array<float, 3>>& vertices) {
    std::array<float, 3> barycenter = { 0.0f, 0.0f, 0.0f };
    for (const auto& vertex : vertices) {
        barycenter[0] += vertex[0];
        barycenter[1] += vertex[1];
        barycenter[2] += vertex[2];
    }
    barycenter[0] /= vertices.size();
    barycenter[1] /= vertices.size();
    barycenter[2] /= vertices.size();
    return barycenter;
}


std::pair<std::array<float, 3>, std::array<float, 3>> computeBoundingBox(const std::vector<std::array<float, 3>>& vertices) {
    std::array<float, 3> min = { FLT_MAX, FLT_MAX, FLT_MAX };
    std::array<float, 3> max = { FLT_MIN, FLT_MIN, FLT_MIN };

    for (const auto& vertex : vertices) {
        for (int i = 0; i < 3; i++) {
            if (vertex[i] < min[i]) min[i] = vertex[i];
            if (vertex[i] > max[i]) max[i] = vertex[i];
        }
    }
    return { min, max };
}

Eigen::Vector3f computeCentroid(const std::vector<std::array<float, 3>>& vertices) {
    Eigen::Vector3f centroid(0.0f, 0.0f, 0.0f);
    for (const auto& vertex : vertices) {
        centroid[0] += vertex[0];
        centroid[1] += vertex[1];
        centroid[2] += vertex[2];
    }
    centroid /= vertices.size();
    return centroid;
}

void normalizeShape(std::vector<std::array<float, 3>>& vertices) {
    auto barycenter = computeBarycenter(vertices);


    for (auto& vertex : vertices) {
        vertex[0] -= barycenter[0];
        vertex[1] -= barycenter[1];
        vertex[2] -= barycenter[2];
    }

    auto [min, max] = computeBoundingBox(vertices);


    float maxDimension = std::max({ max[0] , min[0] * -1, max[1], min[1] * -1, max[2], min[2] * -1 });


    for (auto& vertex : vertices) {
        vertex[0] /= maxDimension;
        vertex[1] /= maxDimension;
        vertex[2] /= maxDimension;
    }
}


void alignMesh(std::vector<std::array<float, 3>>& vertices) {
    Eigen::Matrix3f covarianceMatrix = Eigen::Matrix3f::Zero();
    Eigen::Vector3f centroid = computeCentroid(vertices);

    for (const auto& vertex : vertices) {
        Eigen::Vector3f centeredVertex(vertex[0] - centroid[0], vertex[1] - centroid[1], vertex[2] - centroid[2]);
        covarianceMatrix += centeredVertex * centeredVertex.transpose();
    }
    covarianceMatrix /= vertices.size();

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> eigenSolver(covarianceMatrix);
    Eigen::Matrix3f eigenvectors = eigenSolver.eigenvectors();

    Eigen::Matrix3f alignmentMatrix;
    alignmentMatrix.col(0) = eigenvectors.col(2);  
    alignmentMatrix.col(1) = eigenvectors.col(1); 
    alignmentMatrix.col(2) = eigenvectors.col(2).cross(eigenvectors.col(1));

    for (auto& vertex : vertices) {
        Eigen::Vector3f v(vertex[0], vertex[1], vertex[2]);
        Eigen::Vector3f alignedV = alignmentMatrix.transpose() * v;
        vertex[0] = alignedV[0];
        vertex[1] = alignedV[1];
        vertex[2] = alignedV[2];
    }

}

void flipMesh(std::vector<std::array<float, 3>>& vertices) {
    Eigen::Matrix3f covarianceMatrix = Eigen::Matrix3f::Zero();
    Eigen::Vector3f centroid = computeCentroid(vertices);

    for (const auto& vertex : vertices) {
        Eigen::Vector3f centeredVertex(vertex[0] - centroid[0], vertex[1] - centroid[1], vertex[2] - centroid[2]);
        covarianceMatrix += centeredVertex * centeredVertex.transpose();
    }
    covarianceMatrix /= vertices.size();

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> eigenSolver(covarianceMatrix);
    Eigen::Matrix3f eigenvectors = eigenSolver.eigenvectors();

    Eigen::Vector3f alignedX = eigenvectors.col(2);
    Eigen::Vector3f alignedY = eigenvectors.col(1);
    Eigen::Vector3f alignedZ = alignedX.cross(alignedY);

    Eigen::Vector3f xAxis(1.0f, 0.0f, 0.0f);
    Eigen::Vector3f yAxis(0.0f, 1.0f, 0.0f);
    Eigen::Vector3f zAxis(0.0f, 0.0f, 1.0f);

    if (alignedX.dot(xAxis) < 0.0f) {
        for (auto& vertex : vertices) {
            vertex[0] = -vertex[0];
        }
    }

    if (alignedY.dot(yAxis) < 0.0f) {
        for (auto& vertex : vertices) {
            vertex[1] = -vertex[1];
        }
    }

    alignedZ = alignedX.cross(alignedY);
    if (alignedZ.dot(zAxis) < 0.0f) {
        for (auto& vertex : vertices) {
            vertex[2] = -vertex[2];
        }
    }
}



float normalizeShapeFile(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream inFile(inputFile);
    std::ofstream outFile(outputFile); 
    std::vector<std::array<float, 3>> vertices;
    std::vector<std::vector<int>> faces;

    std::cout << "reading file: " << inputFile << std::endl;
    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back({ x, y, z });
        }
        else if (prefix == "f") {
            std::vector<int> face;
            std::string vertexInfo;
            while (iss >> vertexInfo) {
                std::istringstream vertexStream(vertexInfo);
                int vertexIndex;
                vertexStream >> vertexIndex;
                face.push_back(vertexIndex);
            }
            faces.push_back(face);
        }
    }

    alignMesh(vertices);
    flipMesh(vertices);
    poseOrientationCheck(vertices);

    normalizeShape(vertices);

    

    float tightfit = findMaxAbsoluteValue(computeBoundingBox(vertices));

    for (const auto& vertex : vertices) {
        outFile << "v " << vertex[0] << " " << vertex[1] << " " << vertex[2] << "\n";
    }
    for (const auto& face : faces) {
        outFile << "f";
        for (const auto& vertexIndex : face) {
            outFile << " " << vertexIndex;
        }
        outFile << "\n";
    }
    
    inFile.close();
    outFile.close();
    
    return tightfit;
}


void poseOrientationCheck(const std::vector<std::array<float, 3>>& vertices) {
    Eigen::Vector3f xAxis(1.0f, 0.0f, 0.0f);
    Eigen::Vector3f yAxis(0.0f, 1.0f, 0.0f);
    Eigen::Vector3f zAxis(0.0f, 0.0f, 1.0f);

    Eigen::Matrix3f covarianceMatrix = Eigen::Matrix3f::Zero();
    Eigen::Vector3f centroid = computeCentroid(vertices);

    for (const auto& vertex : vertices) {
        Eigen::Vector3f centeredVertex(vertex[0] - centroid[0], vertex[1] - centroid[1], vertex[2] - centroid[2]);
        covarianceMatrix += centeredVertex * centeredVertex.transpose();
    }
    covarianceMatrix /= vertices.size();

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> eigenSolver(covarianceMatrix);
    Eigen::Matrix3f eigenvectors = eigenSolver.eigenvectors();

    Eigen::Vector3f alignedX = eigenvectors.col(2);
    Eigen::Vector3f alignedY = eigenvectors.col(1);
    Eigen::Vector3f alignedZ = eigenvectors.col(2).cross(eigenvectors.col(1));

    if (alignedX.dot(xAxis) < 0.0f) {
        std::cout << "Warning: X-axis is misaligned after flip. Consider adjusting." << std::endl;
    }
    if (alignedY.dot(yAxis) < 0.0f) {
        std::cout << "Warning: Y-axis is misaligned after flip. Consider adjusting." << alignedY.dot(yAxis) << std::endl;
    }
    if (alignedZ.dot(zAxis) < 0.0f) {
        std::cout << "Warning: Z-axis is misaligned after flip. Consider adjusting." << alignedZ.dot(zAxis) << std::endl;
    }
}


float verify_origin(const std::string& inputFile) {
    std::ifstream inFile(inputFile);
    std::vector<std::array<float, 3>> vertices;
    std::vector<std::vector<int>> faces;

    std::cout << "reading file: " << inputFile << std::endl;

    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back({ x, y, z });
        }
        else if (prefix == "f") {
            std::vector<int> face;
            std::string vertexInfo;
            while (iss >> vertexInfo) {
                std::istringstream vertexStream(vertexInfo);
                int vertexIndex;
                vertexStream >> vertexIndex;
                face.push_back(vertexIndex);
            }
            faces.push_back(face);
        }
    }
    auto barycenter = computeBarycenter(vertices);

    float distance = std::sqrt(barycenter[0] * barycenter[0] +
        barycenter[1] * barycenter[1] +
        barycenter[2] * barycenter[2]);

    return distance;
}



};