import scipy.spatial as spatial

import sys
import os
from tqdm import tqdm

def handle_file(input_file, output_file):
    points = read_points(input_file)
    ConvexHull = convex_hull(points)
    write_mesh(output_file, ConvexHull)

def convex_hull(points):
    return spatial.ConvexHull(points)

def read_points(filename):
    points = []
    with open(filename, 'r') as f:
        lines = f.readlines()
        for line in lines:
            if line.startswith('v '):
                point_xyz = line.split()[1:]
                points.append([float(point_xyz[0]), float(point_xyz[1]), float(point_xyz[2])])

    return points

def write_mesh(outputfilename, ConvexHull):
    index_mapping = {original_index: new_index for new_index, original_index in enumerate(ConvexHull.vertices)}
    with open(outputfilename, "w") as f:
        for i in ConvexHull.vertices:
            f.write(f"v {str(ConvexHull.points[i][0])} {str(ConvexHull.points[i][1])} {str(ConvexHull.points[i][2])}\n")

        for simplex in ConvexHull.simplices:
            new_indices = [index_mapping[vertex] for vertex in simplex]
            f.write(f"f {str(new_indices[0]+1)} {str(new_indices[1]+1)} {str(new_indices[2]+1)} \n")

input_dir = "../../data/OkayMeshes3"
output_dir = "../../res/convex_hulls_okaymeshes3"

if __name__ == "__main__":
    # Iterate through all directories in input_dir
    for dir in os.listdir(input_dir):
        print(f"Handling directory {dir}")
        # Iterate through all files in the directory
        for file in tqdm(os.listdir(os.path.join(input_dir, dir))):
            input_file = os.path.join(input_dir, dir, file)
            # check if input_file ends with .obj
            if input_file.endswith(".obj"):
                output_file = os.path.join(output_dir, dir, file)
                handle_file(input_file, output_file)
