import pandas as pd
import numpy as np
from sklearn.preprocessing import StandardScaler
from sklearn.manifold import TSNE
from sklearn.preprocessing import LabelEncoder
from annoy import AnnoyIndex
from sklearn.decomposition import PCA
import plotly.express as px
import matplotlib.colors as mcolors
#from google.colab import drive

# Mount Google Drive
#drive.mount('/content/drive')

# Read features
features = pd.read_csv('/content/drive/My Drive/features_final.txt', sep='\t')

# Feature names
scalar_feature_names = ['Diameter', 'BB_Diameter', 'BB_Volume', 'SurfaceArea',
                        'Volume', 'VolumeComps', 'Convexity', 'Eccentricity02',
                        'Eccentricity01', 'Eccentricity12', 'Compactness', 'Sphericity',
                        'CubeRootVolume', 'SquareRootArea', 'DiameterToCubeRootVolume',
                        'DiameterToSquareRootArea', 'DiameterSquared', 'BBVolumeCubeRoot',
                        'Rectangularity']

scalar_features = features[scalar_feature_names]

histogram_A3 = features.iloc[:, 21:121]
histogram_D1 = features.iloc[:, 121:221]
histogram_D2 = features.iloc[:, 221:321]
histogram_D3 = features.iloc[:, 321:421]
histogram_D4 = features.iloc[:, 421:521]

# Standardize scalar features
scalar_scaler = StandardScaler()
scalar_features_standardized = pd.DataFrame(
    scalar_scaler.fit_transform(scalar_features),
    columns=scalar_feature_names
)

# Normalize histograms per shape (L1 normalization)
histogram_A3_normalized = histogram_A3.div(histogram_A3.sum(axis=1), axis=0)
histogram_D1_normalized = histogram_D1.div(histogram_D1.sum(axis=1), axis=0)
histogram_D2_normalized = histogram_D2.div(histogram_D2.sum(axis=1), axis=0)
histogram_D3_normalized = histogram_D3.div(histogram_D3.sum(axis=1), axis=0)
histogram_D4_normalized = histogram_D4.div(histogram_D4.sum(axis=1), axis=0)

# Concatenate histograms
histogram_features = pd.concat([
    histogram_A3_normalized,
    histogram_D1_normalized,
    histogram_D2_normalized,
    histogram_D3_normalized,
    histogram_D4_normalized
], axis=1)

# Standardize histogram features per bin across all shapes
histogram_scaler = StandardScaler()
histogram_features_standardized = pd.DataFrame(
    histogram_scaler.fit_transform(histogram_features),
    columns=histogram_features.columns
)

# Concatenate scalar and histogram features to form the final feature vectors
features_standardized = pd.concat([scalar_features_standardized, histogram_features_standardized], axis=1)

# Step 5: Build a KNN engine using Annoy

num_features = features_standardized.shape[1]

# Create an AnnoyIndex (using Euclidean distance)
t = AnnoyIndex(num_features, 'euclidean')

# Add items to the index
for i in range(len(features_standardized)):
    v = features_standardized.iloc[i].values.astype('float32')
    t.add_item(i, v)

# Build the index with a specified number of trees
n_trees = 10  # Adjust n_trees for better accuracy
t.build(n_trees)

# Step 5: Given a query shape, find its K nearest neighbors and perform range search

# Select query indices (choose shapes from different classes)
query_indices = [0, 50, 100, 150, 200]

K = 6  # Number of nearest neighbors to retrieve
R = 5.0  # Distance threshold for range search (adjust based on your data)

results_knn = []
results_range = []

for q_idx in query_indices:
    # Get the query vector
    query_vector = features_standardized.iloc[q_idx].values.astype('float32')
    
    # KNN search
    nearest_neighbors = t.get_nns_by_item(q_idx, K+1, include_distances=True)
    indices_knn = nearest_neighbors[0]
    distances_knn = nearest_neighbors[1]
    # Exclude the query shape itself
    if indices_knn[0] == q_idx:
        indices_knn = indices_knn[1:]
        distances_knn = distances_knn[1:]
    else:
        indices_knn = indices_knn[:K]
        distances_knn = distances_knn[:K]
    # Get the query shape information
    query_shape = features.iloc[q_idx]
    # Get the nearest neighbor shapes
    nn_shapes_knn = features.iloc[indices_knn]
    nn_shapes_knn = nn_shapes_knn.copy()
    nn_shapes_knn['Distance'] = distances_knn
    results_knn.append({'Query': query_shape, 'Neighbors': nn_shapes_knn})
    
    # Range search approximation
    # Annoy does not support range search directly, so we retrieve more neighbors and filter
    possible_neighbors = t.get_nns_by_item(q_idx, n=1000, include_distances=True)
    indices_range = []
    distances_range = []
    for idx, dist in zip(possible_neighbors[0], possible_neighbors[1]):
        if idx != q_idx and dist <= R:
            indices_range.append(idx)
            distances_range.append(dist)
        elif dist > R:
            break  # Since distances are sorted, we can break early
    # Get the range neighbor shapes
    rn_shapes = features.iloc[indices_range]
    rn_shapes = rn_shapes.copy()
    rn_shapes['Distance'] = distances_range
    results_range.append({'Query': query_shape, 'Neighbors': rn_shapes})

# Display the KNN results
print("KNN Search Results:")
for res in results_knn:
    query_shape = res['Query']
    nn_shapes = res['Neighbors']
    print("Query Shape: ClassName={}, Filename={}".format(query_shape['ClassName'], query_shape['Filename']))
    print("Nearest Neighbors:")
    for idx, row in nn_shapes.iterrows():
        print("ClassName={}, Filename={}, Distance={}".format(row['ClassName'], row['Filename'], row['Distance']))
    print("-" * 40)

# Display the Range Search results
print("Range Search Results:")
for res in results_range:
    query_shape = res['Query']
    rn_shapes = res['Neighbors']
    print("Query Shape: ClassName={}, Filename={}".format(query_shape['ClassName'], query_shape['Filename']))
    print("Neighbors within distance R={}:".format(R))
    for idx, row in rn_shapes.iterrows():
        print("ClassName={}, Filename={}, Distance={}".format(row['ClassName'], row['Filename'], row['Distance']))
    print("-" * 40)

# Step 6: Implement t-SNE for dimensionality reduction

X = features_standardized.values

# Optional: Reduce dimensionality with PCA before t-SNE to speed up computation
# pca = PCA(n_components=50)
# X_pca = pca.fit_transform(X)

# Apply t-SNE
tsne = TSNE(n_components=2, perplexity=50, n_iter=6000, random_state=42)
X_embedded = tsne.fit_transform(X)

# Get class labels for coloring the scatter plot
classes = features['ClassName']
le = LabelEncoder()
class_labels = le.fit_transform(classes)

# Create a DataFrame for plotting
df_tsne = pd.DataFrame({
    't-SNE1': X_embedded[:, 0],
    't-SNE2': X_embedded[:, 1],
    'Class': classes,
    'Filename': features['Filename']
})

# Generate a color palette with as many colors as classes
num_classes = len(le.classes_)
colors = [mcolors.hsv_to_rgb([x/num_classes, 0.7, 0.9]) for x in range(num_classes)]
colors_hex = [mcolors.to_hex(c) for c in colors]

# Plot the t-SNE embedding using Plotly with hover annotations
fig = px.scatter(
    df_tsne,
    x='t-SNE1',
    y='t-SNE2',
    color='Class',
    hover_data=['Filename'],
    title='t-SNE visualization of 3D shape features',
    width=800,
    height=600,
    color_discrete_sequence=colors_hex
)
fig.update_layout(legend=dict(itemsizing='constant'))
fig.show()
