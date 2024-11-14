#include "NormalizeMeshes.h"

#include <filesystem>

NormalizeMeshes::NormalizeMeshes(std::string& mesh_directory, std::string& output_directory) {
	m_mesh_directory = mesh_directory;
	m_output_dir = output_directory;
}

NormalizeMeshes::~NormalizeMeshes() {
}

void NormalizeMeshes::handle_all_directories() {
    std::ofstream myfile;
    myfile.open("shapes_normalized_tightfit.csv", std::ios::out | std::ios::app);
    myfile << "class; scale_check ; X ; Y ; Z\n";

    Normalization normalizer;

    std::ofstream myfile1;
    myfile1.open("shapes_normalized_dist.csv", std::ios::out | std::ios::app);
    myfile1 << "class; aligned_on_x ; aligned_on_y ; aligned_on_z ; flipped_on_x ; flipped_on_y ; flipped_on_z\n";
    for (const auto& entry : std::filesystem::directory_iterator(m_mesh_directory)) {
        if (std::filesystem::is_directory(entry.path())) {
            std::cout << "Analyzing category: " << entry.path().filename().string() << std::endl;

            // Iterate through each file in the directory
            for (const auto& file : std::filesystem::directory_iterator(entry.path())) {
                if (file.path().extension() == ".obj") {
                    std::cout << "NORMALIZING STEP!" << std::endl;
                    auto [scale, box, flippedOn, alignedOn] = normalizer.normalizeShapeFile(file.path().string(), m_output_dir + "/" + entry.path().filename().string() + "/" + file.path().filename().string());
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

}