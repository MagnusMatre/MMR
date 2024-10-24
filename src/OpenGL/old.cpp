//////
//////float verify_origin(const std::string& inputFile) {
//////    std::ifstream inFile(inputFile);
//////    std::vector<std::array<float, 3>> vertices;
//////    std::vector<std::vector<int>> faces;
//////
//////    std::cout << "reading file: " << inputFile << std::endl;
//////
//////    std::string line;
//////    while (std::getline(inFile, line)) {
//////        std::istringstream iss(line);
//////        std::string prefix;
//////        iss >> prefix;
//////
//////        if (prefix == "v") {
//////            float x, y, z;
//////            iss >> x >> y >> z;
//////            vertices.push_back({ x, y, z });
//////        }
//////        else if (prefix == "f") {
//////            std::vector<int> face;
//////            std::string vertexInfo;
//////            while (iss >> vertexInfo) {
//////                std::istringstream vertexStream(vertexInfo);
//////                int vertexIndex;
//////                vertexStream >> vertexIndex;
//////                face.push_back(vertexIndex);
//////            }
//////            faces.push_back(face);
//////        }
//////    }
//////    auto barycenter = computeBarycenter(vertices);
//////
//////    float distance = std::sqrt(barycenter[0] * barycenter[0] +
//////        barycenter[1] * barycenter[1] +
//////        barycenter[2] * barycenter[2]);
//////
//////    return distance;
//////}
////
////
////void momentFlipCheck(std::vector<std::array<float, 3>>& vertices, std::vector<std::array<int, 3>>& faces) {
////    std::array<float, 3> moments = { 0.0f, 0.0f, 0.0f };
////
////    for (const auto& face : faces) {
////        std::array<float, 3> centroid = { 0.0f, 0.0f, 0.0f };
////
////        bool validFace = true;
////        for (int i = 0; i < 3; ++i) {
////            if (face[i] <= 0 || face[i] >= vertices.size() + 1) {
////                std::cerr << "Error: Vertex index out of range. Skipping face." << vertices.size() << std::endl;
////                validFace = false;
////                break;
////            }
////        }
////        if (!validFace) continue;
////
////        for (int i = 0; i < 3; ++i) {
////            centroid[0] += vertices[face[i] - 1][0];
////            centroid[1] += vertices[face[i] - 1][1];
////            centroid[2] += vertices[face[i] - 1][2];
////        }
////        centroid[0] /= 3.0f;
////        centroid[1] /= 3.0f;
////        centroid[2] /= 3.0f;
////
////        // Calculate the second-order moments for each axis
////        for (int i = 0; i < 3; ++i) {
////            float signCt = (centroid[i] > 0) ? 1.0f : -1.0f;
////            moments[i] += signCt * std::pow(centroid[i], 2);
////            std::cout << "MOMENT VALUE:" << moments[i];
////        }
////        
////    }
////    for (int i = 0; i < 3; ++i) {
////        if (moments[i] < 0.0f) {
////            for (auto& vertex : vertices) {
////                vertex[i] = -vertex[i];
////            }
////        }
////    }
////}
//
//
//
//
//std::pair<std::array<float, 3>, std::array<float, 3>>  CheckBoundingBox(const std::string& inputFile) {
//    std::ifstream inFile(inputFile);
//    std::vector<std::array<float, 3>> vertices;
//    std::vector<std::array<int, 3>> faces;
//
//    std::cout << "reading file: " << inputFile << std::endl;
//
//    std::string line;
//    while (std::getline(inFile, line)) {
//        std::istringstream iss(line);
//        std::string prefix;
//        iss >> prefix;
//
//        if (prefix == "v") {
//            float x, y, z;
//            iss >> x >> y >> z;
//            vertices.push_back({ x, y, z });
//        }
//        else if (prefix == "f") {
//            int x, y, z;
//            iss >> x >> y >> z;
//            faces.push_back({ x, y, z });
//        }
//    }
//    return computeBoundingBox(vertices);
