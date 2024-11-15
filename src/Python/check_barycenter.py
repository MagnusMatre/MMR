import os
import sys
import pandas as pd
import matplotlib.pyplot as plt
from tqdm import tqdm
import numpy as np
import seaborn as sns

mesh_folder = "../../data/Meshes"

#barycenter_distances = []

# iterate over all directories in the mesh folder directories
# for class_name in tqdm(os.listdir(mesh_folder)):
#     class_folder = os.path.join(mesh_folder, class_name)

#     if not os.path.isdir(class_folder): continue

#     # iterate over all files in the class folder
#     for file_name in os.listdir(class_folder):
#         file_path = os.path.join(class_folder, file_name)


#         if not file_name.endswith(".obj"): continue

#         with open(file_path, "r") as file:
#             lines = file.readlines()
#             vertices = []


#             # read the vertices from the file
#             for line in lines:
#                 if line.startswith("v "):
#                     content = line.split()[1:]
#                     try:
#                         vertices.append(tuple(float(x) for x in content))
#                     except:
#                         print(content)

#             # calculate the barycenter
#             barycenter = [sum([v[i] for v in vertices]) / len(vertices) for i in range(3)]

#             barycenter_distances.append(sum(barycenter[x]**2 for x in range(3))**0.5)

# np.savetxt("../../res/barycenter_distances.txt", barycenter_distances)

barycenter_distances = np.loadtxt("../../res/barycenter_distances.txt")

barycenter_distances = np.log10(barycenter_distances)
print(len(barycenter_distances))

new_barycenter_distances = pd.read_csv("../../res/shapes_normalized_dist.csv", delimiter=";")
print(new_barycenter_distances)
new_barycenter_distances = np.log10(new_barycenter_distances[" dist_from_bary"])

plt.figure(figsize=(10, 6))
plt.hist(barycenter_distances, color="red", label="before", range=(-10, 8), bins=int(np.sqrt(len(barycenter_distances))), edgecolor='black')
plt.hist(new_barycenter_distances, color="blue", label="after", range=(-10, 8), bins=int(np.sqrt(len(barycenter_distances))), edgecolor='black')
plt.xlim(-10, 8)
plt.xlabel(r"$\log_{10}$(distance) to histogram")
plt.ylabel("Number of meshes")
plt.legend()
plt.show()