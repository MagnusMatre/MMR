import numpy as np
import pandas as pd
import os
import trimesh
from scipy.spatial.distance import pdist

def compute_volumes(mesh):
    volume_approximated = 0
    V = abs(mesh.volume)
    convex_hull = mesh.convex_hull
    V_convex = abs(convex_hull.volume)
    obb = mesh.bounding_box_oriented
    V_obb = abs(obb.volume) 
    pitch = 0.05
    if V > V_convex or V > V_obb:
        voxelized = mesh.voxelized(pitch)
        V = voxelized.volume
        volume_approximated = 1  
    return V, V_convex, V_obb, volume_approximated

def compute_surface_area(mesh):
    return mesh.area

def compute_compactness(mesh, V):
    S = mesh.area
    if V == 0:
        return np.nan  
    c = (S ** 3) / (36 * np.pi * V ** 2)
    return c

def compute_rectangularity(mesh, V, V_obb):
    if V_obb == 0:
        return np.nan  
    rectangularity = V / V_obb
    return rectangularity

def compute_diameter(mesh, num_samples=1000):
    points = mesh.sample(num_samples)
    distances = pdist(points)
    diameter = np.max(distances)
    return diameter

def compute_convexity(mesh, V, V_convex):
    try:
        if V_convex == 0:
            return np.nan  
        convexity = V / V_convex
    except Exception as e:
        print(f"Failed to compute convexity: {e}")
        convexity = np.nan
    return convexity

def compute_eccentricity(mesh):
    vertices = mesh.vertices
    centroid = vertices.mean(axis=0)
    centered_vertices = vertices - centroid
    covariance_matrix = np.cov(centered_vertices.T)
    eigenvalues = np.linalg.eigvalsh(covariance_matrix)
    eigenvalues = np.sort(eigenvalues)[::-1]  
    if eigenvalues[-1] == 0:
        return np.nan  
    eccentricity = eigenvalues[0] / eigenvalues[-1]  # Ratio of largest to smallest eigenvalue
    return eccentricity

# def compute_approx_volume(mesh, pitch=0.1):
#     try:
#         voxelized = mesh.voxelized(pitch)
#         volume = voxelized.volume
#         return abs(volume)
#     except Exception as e:
#         print(f"Failed to approximate volume: {e}")
#         return np.nan

# Functions to compute shape property descriptors #
def compute_a3(mesh, num_samples=1000, bins=10):
    angles = []
    vertices = mesh.vertices
    for _ in range(num_samples):
        points = vertices[np.random.choice(len(vertices), 3, replace=False)]
        v1, v2, v3 = points[0], points[1], points[2]
        a = np.linalg.norm(v2 - v1)
        b = np.linalg.norm(v3 - v1)
        c = np.linalg.norm(v3 - v2)
        cos_angle = (a**2 + b**2 - c**2) / (2 * a * b)
        angle = np.arccos(np.clip(cos_angle, -1.0, 1.0))
        angles.append(angle)
    histogram, _ = np.histogram(angles, bins=bins, range=(0, np.pi))
    return histogram

def compute_d1(mesh, num_samples=1000, bins=10):
    distances = []
    vertices = mesh.vertices
    centroid = vertices.mean(axis=0)
    for _ in range(num_samples):
        random_vertex = vertices[np.random.choice(len(vertices))]
        distance = np.linalg.norm(random_vertex - centroid)
        distances.append(distance)
    histogram, _ = np.histogram(distances, bins=bins)
    return histogram

def compute_d2(mesh, num_samples=1000, bins=10):
    distances = []
    vertices = mesh.vertices
    for _ in range(num_samples):
        v1, v2 = vertices[np.random.choice(len(vertices), 2, replace=False)]
        distance = np.linalg.norm(v2 - v1)
        distances.append(distance)
    histogram, _ = np.histogram(distances, bins=bins)
    return histogram

def compute_d3(mesh, num_samples=1000, bins=10):
    areas = []
    vertices = mesh.vertices
    for _ in range(num_samples):
        points = vertices[np.random.choice(len(vertices), 3, replace=False)]
        v1, v2, v3 = points[0], points[1], points[2]
        area = 0.5 * np.linalg.norm(np.cross(v2 - v1, v3 - v1))
        areas.append(np.sqrt(area))
    histogram, _ = np.histogram(areas, bins=bins)
    return histogram

def compute_d4(mesh, num_samples=1000, bins=10):
    volumes = []
    vertices = mesh.vertices
    for _ in range(num_samples):
        points = vertices[np.random.choice(len(vertices), 4, replace=False)]
        v1, v2, v3, v4 = points[0], points[1], points[2], points[3]
        volume = np.abs(np.dot(np.cross(v2 - v1, v3 - v1), v4 - v1)) / 6.0
        volumes.append(np.cbrt(volume))
    histogram, _ = np.histogram(volumes, bins=bins)
    return histogram

def compute_features(mesh):
    
    V, V_convex, V_obb, volume_approximated = compute_volumes(mesh)
    
    if mesh.is_watertight:
        watertight = 1
    else:
        watertight = 0
    
    features = {
        'surface_area': compute_surface_area(mesh),
        'compactness': compute_compactness(mesh, V),
        'rectangularity': compute_rectangularity(mesh, V, V_obb),
        'diameter': compute_diameter(mesh),
        'convexity': compute_convexity(mesh, V, V_convex),
        'eccentricity': compute_eccentricity(mesh),
        'watertight' : watertight, 
        'volume_approximated': volume_approximated, 
        'a3_histogram': compute_a3(mesh),
        'd1_histogram': compute_d1(mesh),
        'd2_histogram': compute_d2(mesh),
        'd3_histogram': compute_d3(mesh),
        'd4_histogram': compute_d4(mesh)
    }
    return features

def process_dataset(dataset_path):
    data = []
    for root, dirs, files in os.walk(dataset_path):
        for file in files:
            if file.endswith('.obj'):
                file_path = os.path.join(root, file)
                mesh = trimesh.load(file_path, force='mesh')
                if len(mesh.vertices) == 0 or len(mesh.faces) == 0:
                    print(f"Skipping empty mesh: {file}")
                    continue
                try:
                    features = compute_features(mesh)
                    features['file'] = file
                    features['class'] = os.path.basename(root)
                    data.append(features)
                except Exception as e:
                    print(f"Skipping mesh due to error: {file}, Error: {e}")
    df = pd.DataFrame(data)
    return df

dataset_path = os.path.abspath(r'C:\Users\masou\OneDrive\Desktop\Utrecht University\Multimedia Retrieval\Assignment\Step3\dataset')
df_features = process_dataset(dataset_path)
print(df_features.head())
csv_save_path = os.path.join(dataset_path, 'features_result.csv')
df_features.to_csv(csv_save_path, index=False)
len(df_features[df_features['convexity']>1])