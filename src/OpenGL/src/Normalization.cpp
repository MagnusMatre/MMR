#include "Normalization.h"

Normalization::Normalization() {
}

Normalization::~Normalization() {
}

float Normalization::findMaxAbsoluteValue(const std::pair<std::array<float, 3>, std::array<float, 3>>& list) {
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

std::array<float, 3> Normalization::computeBarycenter(const std::vector<std::array<float, 3>>& vertices) {
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


std::pair<std::array<float, 3>, std::array<float, 3>> Normalization::computeBoundingBox(const std::vector<std::array<float, 3>>& vertices) {
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

Eigen::Vector3f Normalization::computeCentroid(const std::vector<std::array<float, 3>>& vertices) {
    Eigen::Vector3f centroid(0.0f, 0.0f, 0.0f);
    for (const auto& vertex : vertices) {
        centroid[0] += vertex[0];
        centroid[1] += vertex[1];
        centroid[2] += vertex[2];
    }
    centroid /= vertices.size();
    return centroid;
}

void Normalization::normalizeShape(std::vector<std::array<float, 3>>& vertices) {
    auto barycenter = computeBarycenter(vertices);


    for (auto& vertex : vertices) {
        vertex[0] -= barycenter[0];
        vertex[1] -= barycenter[1];
        vertex[2] -= barycenter[2];
    }

    auto [min, max] = computeBoundingBox(vertices);

    //Switch so the total volume is 1
    //float maxDimension = std::max({ max[0] , min[0] * -1, max[1], min[1] * -1, max[2], min[2] * -1 });

    //for (auto& vertex : vertices) {
    //    vertex[0] /= maxDimension;
    //    vertex[1] /= maxDimension;
    //    vertex[2] /= maxDimension;
    //}

    float X = max[0] - min[0];
    float Y = max[1] - min[1];
    float Z = max[2] - min[2];

    float currentVolume = (max[0] - min[0]) * (max[1] - min[1]) * (max[2] - min[2]);

    float t = std::max(X, Y);
    float t_1 = std::max(t, Z);

    float scaleFactor = 1.0f / t_1;

    // Apply the scale factor to normalize the shape
    for (auto& vertex : vertices) {
        vertex[0] *= scaleFactor;
        vertex[1] *= scaleFactor;
        vertex[2] *= scaleFactor;
    }

}


void Normalization::alignMesh(std::vector<std::array<float, 3>>& vertices) {
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

    Eigen::Vector3f yAxis(0.0f, 1.0f, 0.0f);
    Eigen::Vector3f zAxis(0.0f, 0.0f, 1.0f);

    for (auto& vertex : vertices) {
        Eigen::Vector3f v(vertex[0], vertex[1], vertex[2]);
        Eigen::Vector3f alignedV = alignmentMatrix.transpose() * v;
        vertex[0] = alignedV[0];
        vertex[1] = alignedV[1];
        vertex[2] = alignedV[2];
    }

}

std::array<float, 3> Normalization::flipFaces(const std::vector<std::array<float, 3>>& vertices, std::vector<std::array<int, 3>>& faces) {
    std::array<float, 3> moments = { 0.0f, 0.0f, 0.0f };
    for (const auto& face : faces) {
        std::array<float, 3> centroid = { 0.0f, 0.0f, 0.0f };
        bool validFace = true;
        for (int i = 0; i < 3; ++i) {
            if (face[i] < 0 || face[i] >= vertices.size()) {
                //std::cerr << "Error: Vertex index out of range. Skipping face." << std::endl;
                validFace = false;
                break;
            }
        }
        if (!validFace) continue;


        for (int i = 0; i < 3; ++i) {
            centroid[0] += vertices[face[i] - 1][0];
            centroid[1] += vertices[face[i] - 1][1];
            centroid[2] += vertices[face[i] - 1][2];
        }
        centroid[0] /= 3.0f;
        centroid[1] /= 3.0f;
        centroid[2] /= 3.0f;

        // Calculate the second-order moments for each axis
        for (int i = 0; i < 3; ++i) {
            float signCt = (centroid[i] > 0) ? 1.0f : -1.0f;
            moments[i] += signCt * std::pow(centroid[i], 2);

        }

    }
    return moments;
}

void Normalization::momentFlip(std::vector<std::array<float, 3>>& vertices, std::vector<std::array<int, 3>>& faces) {
    std::array<float, 3> moments = flipFaces(vertices, faces);
    std::cout << "MOMENT VALUE:" << moments[0] << " " << moments[1] << " " << moments[2] << std::endl;
    for (int i = 0; i < 3; ++i) {
        if (moments[i] < 0.0f) {
            for (auto& vertex : vertices) {
                vertex[i] = -vertex[i];
            }
            std::cout << "Flipped on axis : " << i << " Moment: " << moments[i] << std::endl;
        }
    }
}





std::tuple<bool, std::array<float, 3>, std::array<bool, 3>, std::array<bool, 3>> Normalization::normalizeShapeFile(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream inFile(inputFile);
    std::ofstream outFile(outputFile);
    std::cout << "saving to: " << outputFile << std::endl;
    std::vector<std::array<float, 3>> vertices;
    std::vector<std::array<int, 3>> faces;

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
            int x, y, z;
            iss >> x >> y >> z;
            faces.push_back({ x, y, z });
        }
    }
    std::vector<std::array<float, 3>> originalVertices = vertices;
    alignMesh(vertices);
    momentFlip(vertices, faces);
    std::vector<std::array<float, 3>> flippedVertices = vertices;
    std::cout << (originalVertices == flippedVertices);
    auto alignment = verifyMeshAlignmentAndFlip(flippedVertices, faces);
    std::array<bool, 3> flippedOn = alignment[0];
    std::array<bool, 3> alignedOn = alignment[1];

    normalizeShape(vertices);

    auto [min, max] = computeBoundingBox(vertices);
    float X = max[0] - min[0];
    float Y = max[1] - min[1];
    float Z = max[2] - min[2];
    std::array<float, 3> box = { X, Y, Z };


    float epsilon = 0.00001;
    bool fitCheck = (abs(X) <= 1 + epsilon && abs(Y) <= 1 + epsilon && abs(Z) <= 1 + epsilon);
    bool tightCheck = (abs(X) - 1 <= epsilon || abs(Y) - 1 <= epsilon || abs(Z) - 1 <= epsilon);


    bool scalingCheck = fitCheck && tightCheck;



    //float tightfit = findMaxAbsoluteValue(computeBoundingBox(vertices));

    std::cout << X << ", " << Y << ", " << Z << ", Scale: " << scalingCheck << std::endl;


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

    return { scalingCheck, box , flippedOn, alignedOn };
}


float Normalization::roundIfCloseToOne(float value, float min_threshold = 0.99, float max_threshold = 1.1) {
    // If the value is between the threshold and 1.0, round it to 1.0
    if (value >= min_threshold && value < 1.0f) {
        return 1.0f;
    }
    if (value <= max_threshold && value > 1.0f) {
        return 1.0f;
    }
    return value;
}
const std::vector<std::array<bool, 3>> Normalization::verifyMeshAlignmentAndFlip(const std::vector<std::array<float, 3>>& vertices, std::vector<std::array<int, 3>>& faces) {
    std::array<bool, 3> flippedOn = { false, false, false };
    std::array<bool, 3> alignedOn = { true, true, true };

    Eigen::Matrix3f covarianceMatrix = Eigen::Matrix3f::Zero();
    Eigen::Vector3f centroid = computeCentroid(vertices);

    for (const auto& vertex : vertices) {
        Eigen::Vector3f centeredVertex(vertex[0] - centroid[0], vertex[1] - centroid[1], vertex[2] - centroid[2]);
        covarianceMatrix += centeredVertex * centeredVertex.transpose();
    }
    covarianceMatrix /= vertices.size();

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> eigenSolver(covarianceMatrix);
    Eigen::Matrix3f eigenvectors = eigenSolver.eigenvectors();

    Eigen::Vector3f xAxis(1.0f, 0.0f, 0.0f);
    Eigen::Vector3f yAxis(0.0f, 1.0f, 0.0f);
    Eigen::Vector3f zAxis(0.0f, 0.0f, 1.0f);


    Eigen::Vector3f alignedX = eigenvectors.col(2);
    Eigen::Vector3f alignedY = eigenvectors.col(1);
    Eigen::Vector3f alignedZ = eigenvectors.col(2).cross(eigenvectors.col(1));

    std::cout << "DOT PRODUCT" << abs(alignedX.dot(xAxis)) << std::endl;
    float epsilon = 0.00001;

    //std::cout << "ROUNDING UP!!!!:" << roundUpIfCloseToOne(0.98f, 0.99f) << std::endl;

    if (roundIfCloseToOne(abs(alignedX.dot(xAxis)), 0.99f, 1.01f) != 1.0f) {
        std::cout << "X-axis is not aligned properly." << abs(alignedX.dot(xAxis)) << std::endl;
        alignedOn[0] = false;
    }
    if (roundIfCloseToOne(abs(alignedY.dot(yAxis)), 0.99f, 1.01f) != 1.0f) {
        std::cout << "Y-axis is not aligned properly." << abs(alignedY.dot(yAxis)) << std::endl;
        alignedOn[1] = false;
    }
    if (roundIfCloseToOne(abs(alignedZ.dot(zAxis)), 0.99f, 1.01f) != 1.0f) {
        std::cout << "Z-axis is not aligned properly." << abs(alignedZ.dot(zAxis)) << std::endl;
        alignedOn[2] = false;
    }

    std::array<float, 3> moments = flipFaces(vertices, faces);

    std::cout << "MOMENT VALUE OF NORMALIZED SHAPE:" << moments[0] << " " << moments[1] << " " << moments[2] << std::endl;

    for (int i = 0; i < 3; ++i) {
        if (moments[i] + epsilon < 0.0f) {
            std::cout << "Mesh is flipped along axis " << i << ". Consider mirroring." << std::endl;
        }
        else {
            std::cout << "Mesh is correctly aligned along axis " << i << "." << std::endl;
            flippedOn[i] = true;

        }
    }
    return { flippedOn, alignedOn };
};


const std::vector<std::array<bool, 3>> Normalization::alignmentAndFlipCheck(const std::string& inputFile) {
    std::ifstream inFile(inputFile);
    std::vector<std::array<float, 3>> vertices;
    std::vector<std::array<int, 3>> faces;

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
            int x, y, z;
            iss >> x >> y >> z;
            faces.push_back({ x, y, z });
        }
    }
    return verifyMeshAlignmentAndFlip(vertices, faces);

}




std::pair<std::array<float, 3>, std::array<float, 3>>  Normalization::CheckBoundingBox(const std::string& inputFile) {
    std::ifstream inFile(inputFile);
    std::vector<std::array<float, 3>> vertices;
    std::vector<std::array<int, 3>> faces;

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
            int x, y, z;
            iss >> x >> y >> z;
            faces.push_back({ x, y, z });
        }
    }
    return computeBoundingBox(vertices);

}

const std::pair<std::vector<std::array<float, 3>>, std::vector<std::array<int, 3>> > Normalization::get_vertices_faces(const std::string& inputFile) {
    std::ifstream inFile(inputFile);
    std::vector<std::array<float, 3>> vertices;
    std::vector<std::array<int, 3>> faces;

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
            int x, y, z;
            iss >> x >> y >> z;
            faces.push_back({ x, y, z });
        }
    }
    return { vertices, faces };

}