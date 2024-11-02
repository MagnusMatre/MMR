import pandas as pd
import numpy as np
from sklearn.preprocessing import StandardScaler
from sklearn.manifold import TSNE
from sklearn.preprocessing import LabelEncoder
from annoy import AnnoyIndex
from sklearn.decomposition import PCA
import plotly.express as px
import matplotlib.colors as mcolors

features = pd.read_csv('/content/drive/My Drive/features_final.txt', sep='\t')

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

scalar_scaler = StandardScaler()
scalar_features_standardized = pd.DataFrame(
    scalar_scaler.fit_transform(scalar_features),
    columns=scalar_feature_names
)

histogram_A3_normalized = histogram_A3.div(histogram_A3.sum(axis=1), axis=0)
histogram_D1_normalized = histogram_D1.div(histogram_D1.sum(axis=1), axis=0)
histogram_D2_normalized = histogram_D2.div(histogram_D2.sum(axis=1), axis=0)
histogram_D3_normalized = histogram_D3.div(histogram_D3.sum(axis=1), axis=0)
histogram_D4_normalized = histogram_D4.div(histogram_D4.sum(axis=1), axis=0)

histogram_features = pd.concat([
    histogram_A3_normalized,
    histogram_D1_normalized,
    histogram_D2_normalized,
    histogram_D3_normalized,
    histogram_D4_normalized
], axis=1)

histogram_scaler = StandardScaler()
histogram_features_standardized = pd.DataFrame(
    histogram_scaler.fit_transform(histogram_features),
    columns=histogram_features.columns
)

features_standardized = pd.concat([scalar_features_standardized, histogram_features_standardized], axis=1)

num_features = features_standardized.shape[1]

t = AnnoyIndex(num_features, 'euclidean')

for i in range(len(features_standardized)):
    v = features_standardized.iloc[i].values.astype('float32')
    t.add_item(i, v)

n_trees = 10
t.build(n_trees)

query_indices = [0, 50, 100, 150, 200]
K = 6
R = 5.0

results_knn = []
results_range = []

for q_idx in query_indices:
    query_vector = features_standardized.iloc[q_idx].values.astype('float32')
    nearest_neighbors = t.get_nns_by_item(q_idx, K+1, include_distances=True)
    indices_knn = nearest_neighbors[0]
    distances_knn = nearest_neighbors[1]
    if indices_knn[0] == q_idx:
        indices_knn = indices_knn[1:]
        distances_knn = distances_knn[1:]
    else:
        indices_knn = indices_knn[:K]
        distances_knn = distances_knn[:K]
    query_shape = features.iloc[q_idx]
    nn_shapes_knn = features.iloc[indices_knn]
    nn_shapes_knn = nn_shapes_knn.copy()
    nn_shapes_knn['Distance'] = distances_knn
    results_knn.append({'Query': query_shape, 'Neighbors': nn_shapes_knn})
    
    possible_neighbors = t.get_nns_by_item(q_idx, n=1000, include_distances=True)
    indices_range = []
    distances_range = []
    for idx, dist in zip(possible_neighbors[0], possible_neighbors[1]):
        if idx != q_idx and dist <= R:
            indices_range.append(idx)
            distances_range.append(dist)
        elif dist > R:
            break
    rn_shapes = features.iloc[indices_range]
    rn_shapes = rn_shapes.copy()
    rn_shapes['Distance'] = distances_range
    results_range.append({'Query': query_shape, 'Neighbors': rn_shapes})

print("KNN Search Results:")
for res in results_knn:
    query_shape = res['Query']
    nn_shapes = res['Neighbors']
    print("Query Shape: ClassName={}, Filename={}".format(query_shape['ClassName'], query_shape['Filename']))
    print("Nearest Neighbors:")
    for idx, row in nn_shapes.iterrows():
        print("ClassName={}, Filename={}, Distance={}".format(row['ClassName'], row['Filename'], row['Distance']))
    print("-" * 40)

print("Range Search Results:")
for res in results_range:
    query_shape = res['Query']
    rn_shapes = res['Neighbors']
    print("Query Shape: ClassName={}, Filename={}".format(query_shape['ClassName'], query_shape['Filename']))
    print("Neighbors within distance R={}:".format(R))
    for idx, row in rn_shapes.iterrows():
        print("ClassName={}, Filename={}, Distance={}".format(row['ClassName'], row['Filename'], row['Distance']))
    print("-" * 40)

X = features_standardized.values

tsne = TSNE(n_components=2, perplexity=50, n_iter=6000, random_state=42)
X_embedded = tsne.fit_transform(X)

classes = features['ClassName']
le = LabelEncoder()
class_labels = le.fit_transform(classes)

df_tsne = pd.DataFrame({
    't-SNE1': X_embedded[:, 0],
    't-SNE2': X_embedded[:, 1],
    'Class': classes,
    'Filename': features['Filename']
})

num_classes = len(le.classes_)
colors = [mcolors.hsv_to_rgb([x/num_classes, 0.7, 0.9]) for x in range(num_classes)]
colors_hex = [mcolors.to_hex(c) for c in colors]

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
