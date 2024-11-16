#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <string>
#include <Eigen/Dense> 
#include <array>

class Normalization {
public:

    Normalization();
	~Normalization();

    float findMaxAbsoluteValue(const std::pair<std::array<float, 3>, std::array<float, 3>>& list);
    std::array<float, 3> computeBarycenter(const std::vector<std::array<float, 3>>& vertices);
    std::pair<std::array<float, 3>, std::array<float, 3>> computeBoundingBox(const std::vector<std::array<float, 3>>& vertices);
    Eigen::Vector3f computeCentroid(const std::vector<std::array<float, 3>>& vertices);
    void normalizeShape(std::vector<std::array<float, 3>>& vertices);
    void alignMesh(std::vector<std::array<float, 3>>& vertices);
    std::array<float, 3> flipFaces(const std::vector<std::array<float, 3>>& vertices, std::vector<std::array<int, 3>>& faces);
    void momentFlip(std::vector<std::array<float, 3>>& vertices, std::vector<std::array<int, 3>>& faces);
    std::tuple<bool, std::array<float, 3>, std::array<bool, 3>, std::array<bool, 3>> normalizeShapeFile(const std::string& inputFile, const std::string& outputFile);

    const std::vector<std::array<bool, 3>> verifyMeshAlignmentAndFlip(const std::vector<std::array<float, 3>>& vertices, std::vector<std::array<int, 3>>& faces);
    const std::vector<std::array<bool, 3>> alignmentAndFlipCheck(const std::string& inputFile);
    std::pair<std::array<float, 3>, std::array<float, 3>>  CheckBoundingBox(const std::string& inputFile);

    const std::pair<std::vector<std::array<float, 3>>, std::vector<std::array<int, 3>> > get_vertices_faces(const std::string& inputFile);
    float roundIfCloseToOne(float value, float min_threshold, float max_threshold);

};