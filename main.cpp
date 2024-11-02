#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <flann/flann.hpp>
#include <Eigen/Dense>
#include "tsne.h"
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

// Function to read the features from a tab-separated text file
std::vector<std::vector<float>> readFeatures(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::vector<float>> features;
    std::string line;

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string value;
        std::vector<float> row;

        while (getline(ss, value, '\t')) {
            row.push_back(std::stof(value));
        }
        features.push_back(row);
    }

    file.close();
    return features;
}

// Standardize features using Z-score normalization
void standardizeFeatures(Eigen::MatrixXf& features) {
    Eigen::VectorXf mean = features.colwise().mean();
    Eigen::VectorXf stddev = ((features.rowwise() - mean.transpose()).array().square().colwise().sum() / (features.rows() - 1)).sqrt();

    for (int i = 0; i < features.rows(); ++i) {
        features.row(i) = (features.row(i) - mean.transpose()).array() / stddev.transpose().array();
    }
}

// Perform KNN search using FLANN
void knnSearch(const flann::Index<flann::L2<float>>& index, const Eigen::MatrixXf& query, int K) {
    std::vector<std::vector<int>> indices;
    std::vector<std::vector<float>> dists;

    flann::Matrix<float> query_mat((float*)query.data(), query.rows(), query.cols());
    index.knnSearch(query_mat, indices, dists, K + 1, flann::SearchParams(128));

    for (size_t i = 0; i < indices.size(); ++i) {
        std::cout << "Query point " << i << " nearest neighbors:" << std::endl;
        for (size_t j = 1; j < indices[i].size(); ++j) { // Skip the first neighbor since it's the query itself
            std::cout << "Index: " << indices[i][j] << ", Distance: " << dists[i][j] << std::endl;
        }
        std::cout << "-------------------------" << std::endl;
    }
}

// Perform range search approximation using FLANN
void rangeSearch(const flann::Index<flann::L2<float>>& index, const Eigen::MatrixXf& query, float R) {
    std::vector<std::vector<int>> indices;
    std::vector<std::vector<float>> dists;

    flann::Matrix<float> query_mat((float*)query.data(), query.rows(), query.cols());
    index.radiusSearch(query_mat, indices, dists, R, flann::SearchParams(128));

    for (size_t i = 0; i < indices.size(); ++i) {
        std::cout << "Query point " << i << " neighbors within distance R=" << R << ":" << std::endl;
        for (size_t j = 0; j < indices[i].size(); ++j) {
            std::cout << "Index: " << indices[i][j] << ", Distance: " << dists[i][j] << std::endl;
        }
        std::cout << "-------------------------" << std::endl;
    }
}

// Apply t-SNE to reduce dimensions and plot
void applyTSNE(const Eigen::MatrixXf& data, int n_components = 2) {
    size_t n = data.rows();
    size_t dim = data.cols();
    std::vector<double> tsne_data(n * n_components);

    // Convert data to a format suitable for tsne-cpp
    std::vector<double> input_data(n * dim);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            input_data[i * dim + j] = data(i, j);
        }
    }

    TSNE::run(input_data.data(), n, dim, tsne_data.data(), n_components);

    // Extract results and plot using matplotlibcpp
    std::vector<double> x(n), y(n);
    for (size_t i = 0; i < n; ++i) {
        x[i] = tsne_data[i * n_components];
        y[i] = tsne_data[i * n_components + 1];
    }

    plt::scatter(x, y, 10);
    plt::title("t-SNE visualization of 3D shape features");
    plt::show();
}

int main() {
    try {
        // Step 1: Read the features from the file
        std::string filename = "features_final.txt";
        std::vector<std::vector<float>> features_vec = readFeatures(filename);

        // Step 2: Convert features to Eigen matrix
        int num_samples = features_vec.size();
        int num_features = features_vec[0].size();
        Eigen::MatrixXf features(num_samples, num_features);

        for (int i = 0; i < num_samples; ++i) {
            for (int j = 0; j < num_features; ++j) {
                features(i, j) = features_vec[i][j];
            }
        }

        // Step 3: Standardize features
        standardizeFeatures(features);

        // Step 4: Build a KNN engine using FLANN
        flann::Matrix<float> dataset((float*)features.data(), features.rows(), features.cols());
        flann::Index<flann::L2<float>> index(dataset, flann::KDTreeIndexParams(4));
        index.buildIndex();

        // Step 5: Perform KNN search for a few query points
        Eigen::MatrixXf query_points = features.topRows(5);  // Use the first 5 points as queries
        knnSearch(index, query_points, 6);

        // Step 6: Perform range search for the same query points
        float R = 5.0;  // Distance threshold for range search
        rangeSearch(index, query_points, R);

        // Step 7: Apply t-SNE for dimensionality reduction and plot
        applyTSNE(features);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
