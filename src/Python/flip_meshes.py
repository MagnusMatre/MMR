# Iterate through all subdirectories of the directory containing the meshes that need to be flipped

import os
import sys

from tqdm import tqdm

to_flip_directory = "../../data/NormalflipMeshes"
to_flip_list = []

for classname in os.listdir(to_flip_directory):
    class_directory = os.path.join(to_flip_directory, classname)
    for meshname in os.listdir(class_directory):
        mesh_path = os.path.join(class_directory, meshname)
        to_flip_list.append(f"/{classname}/{meshname}")


meshes_to_use_directory = "../../data/shapes_normalized_5"
output_directory = "../../data/OkayMeshes3"

for classname in tqdm(os.listdir(meshes_to_use_directory)):
    class_directory = os.path.join(meshes_to_use_directory, classname)
    for meshname in os.listdir(class_directory):
        mesh_path = os.path.join(class_directory, meshname)
        if f"/{classname}/{meshname}" in to_flip_list:
            # load the mesh file
            new_file_lines = []
            with open(mesh_path, 'r') as f:
                lines = f.readlines()
            
                for line in lines:
                    if line.startswith("v"):
                        new_file_lines.append(line)
                    
                    elif line.startswith("f"):
                        indcs = line.split(" ")[1:]
                        new_indcs = [indcs[0], indcs[2][:-1], indcs[1]]
                        #new_indcs = [indcs[0], indcs[1], indcs[2][:-1]]
                        new_file_lines.append(f"f {new_indcs[0]} {new_indcs[1]} {new_indcs[2]}\n")
            
            # Open a .obj file to write the flipped mesh
            with open(os.path.join(output_directory, classname, meshname), 'w') as f:
                f.writelines(new_file_lines)

