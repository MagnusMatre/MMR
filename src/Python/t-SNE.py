from sklearn.manifold import TSNE

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import matplotlib.colors as mcolors
import plotly.express as px

# load in a feature file and store in a pandas dataframe
features = pd.read_csv("../../res/features_final.txt", sep='\t')

distances = pd.read_csv("../../res/distance_matrix/0_0_0.700000.txt", sep=',', header=None)

# remove the last column
distances = distances.iloc[:, :-1]

print(distances)
#extract labels
labels = features['ClassName']

# randomly select 10 classes from the dataset
np.random.seed(0)

# get the unique class names
class_names = np.unique(labels)

# randomly select 10 classes
random_classes = class_names

# fully select all samples for 10 random classes
features = features.loc[features['ClassName'].isin(random_classes)]

labels = features['ClassName']

print(features)

# drop the descriptive labels from the features
pure_features = features

# drop column "FileName"
pure_features = pure_features.drop(columns=['Filename'])
pure_features = pure_features.drop(columns=["ClassName"])


# ITERATIONS = 5000
RANDOM_STATE = 42
# PERPLEXITY = 30
N_COMPONENTS = 2

#METHOD = "exact"
METHOD = "barnes_hut"

#randomly select 10 classes for which we plot all the points
NUM_RANDOM_CLASSES = 10

np.random.seed(0)
random_classes = np.random.choice(class_names, NUM_RANDOM_CLASSES)

for ITERATIONS in [100000]:
    for PERPLEXITY in [25, 30, 40]:
# for ITERATIONS in [1000, 5000, 10000, 50000]:
#     for PERPLEXITY in [10, 20, 30, 40, 50, 60, 80, 100]:
        # Create a t-SNE model with 2 components
        model = TSNE(n_components=N_COMPONENTS, random_state=RANDOM_STATE, n_iter=ITERATIONS, perplexity=PERPLEXITY, method=METHOD)

        # Fit the model to the features
        #tsne_features = model.fit_transform(pure_features, y=labels)
        tsne_features = model.fit_transform(distances, y=labels)

        df_tsne = pd.DataFrame({
            't-SNE1': tsne_features[:, 0],
            't-SNE2': tsne_features[:, 1],
            'Class': features['ClassName'],
            'Filename': features['Filename']
        })

        #fully select all samples for 10 random classes
        tsne_df_truncated = df_tsne.loc[df_tsne['Class'].isin(random_classes)]

        #df_tsne_truncated = df_tsne.loc[df_tsne['Class'].isin(["Bicycle", "Door", "Wheel", "Bus", "BuildingNonResidential"])]


        # colors = [mcolors.hsv_to_rgb([x/len(class_names), 0.7, 0.9]) for x in range(len(class_names))]
        # colors_hex = [mcolors.to_hex(c) for c in colors]

        colors_truncated = [mcolors.hsv_to_rgb([x/NUM_RANDOM_CLASSES, 0.7, 0.9]) for x in range(NUM_RANDOM_CLASSES)]

        colors = [mcolors.hsv_to_rgb([x//10 * 0.1, 0.7, 0.5 + (x%7)/12]) for x in range(len(class_names))]
        colors_hex_truncated = [mcolors.to_hex(c) for c in colors_truncated]
        colors_hex = [mcolors.to_hex(c) for c in colors]

        fig = px.scatter(
        # df_tsne,
            tsne_df_truncated,
            x='t-SNE1',
            y='t-SNE2',
            color='Class',
            hover_data=['Filename'],
            title=f'10 random classes after {ITERATIONS} iterations with perplexity {PERPLEXITY}',
            width=800,
            height=600,
            color_discrete_sequence=colors_hex_truncated
        )
        fig.update_layout(legend=dict(itemsizing='constant'))
        #fig.show()
        fig.write_image(f"../../res/t-SNE_plots/distances_t-SNE_{ITERATIONS}_{PERPLEXITY}_{RANDOM_STATE}_truncated.png")

        fig2 = px.scatter(
            df_tsne,
            x='t-SNE1',
            y='t-SNE2',
            color='Class',
            hover_data=['Filename'],
            title=f'All classes after {ITERATIONS} iterations with perplexity {PERPLEXITY}',
            width=800,
            height=600,
            color_discrete_sequence=colors_hex
        )
        fig2.update_layout(legend=dict(itemsizing='constant'))
        #fig2.show()
        fig2.write_image(f"../../res/t-SNE_plots/distances_t-SNE_{ITERATIONS}_{PERPLEXITY}_{RANDOM_STATE}_{METHOD}_all.png")
