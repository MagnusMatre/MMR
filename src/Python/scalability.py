import pandas as pd
import numpy as np
from sklearn.preprocessing import StandardScaler
from sklearn.manifold import TSNE
import plotly.express as px

# Load and preprocess data
features = pd.read_csv("C:/Users/masou/OneDrive/Desktop/Utrecht University/Multimedia Retrieval/Assignment/MMR/src/Python/features_final.txt", sep='\t')

# Define feature sets
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

X = features_standardized.values

# t-SNE parameters
perplexity = 60
n_iter = 60000

# Apply t-SNE
tsne = TSNE(n_components=2, perplexity=perplexity, n_iter=n_iter, random_state=42)
X_embedded = tsne.fit_transform(X)

# Get class labels for coloring
classes = features['ClassName']

# Filter for selected classes
selected_classes = ["Bicycle", "Door", "Wheel", "Bus", "BuildingNonResidential"]
filtered_indices = classes.isin(selected_classes)
filtered_indices_array = filtered_indices.values

# Check how many instances are available for each of the selected classes after filtering
filtered_classes = classes[filtered_indices]

# Filter the t-SNE results and features for selected classes only
X_embedded_filtered = X_embedded[filtered_indices_array]
classes_filtered = classes[filtered_indices].reset_index(drop=True)
features_filtered = features[filtered_indices].reset_index(drop=True)

# Verify the filtered data
if len(classes_filtered.unique()) < len(selected_classes):
    print("Warning: Not all selected classes are present after filtering!")

# Prepare DataFrame for Plotly
tsne_df_filtered = pd.DataFrame(X_embedded_filtered, columns=['TSNE Component 1', 'TSNE Component 2'])
tsne_df_filtered['ClassName'] = classes_filtered
tsne_df_filtered['Filename'] = features_filtered['Filename']  # Assuming Filename column exists

# Interactive plot with Plotly
fig = px.scatter(
    tsne_df_filtered, 
    x='TSNE Component 1', 
    y='TSNE Component 2', 
    color='ClassName', 
    opacity=1,  
    hover_data={'Filename': True, 'ClassName': False},
    title=f'Interactive t-SNE Visualization of Selected 3D Shape Features with perplexity {perplexity} and iteration {n_iter}',
    color_discrete_sequence=px.colors.qualitative.Set1  # Set a larger color palette for distinction
)

# Increase marker size to improve visibility
fig.update_traces(marker=dict(size=10, line=dict(width=1.5, color='DarkSlateGrey')))
fig.update_layout(
    legend_title_text='Classes',
    xaxis_title='t-SNE Component 1',
    yaxis_title='t-SNE Component 2',
    width=1000,
    height=600
)

fig.show()
