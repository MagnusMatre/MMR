from sklearn.manifold import TSNE

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# load in a feature file and store in a pandas dataframe
features = pd.read_csv("../../res/features_final.txt", sep='\t')

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


for iterations in [1000]:
    # Create a t-SNE model with 2 components
    model = TSNE(n_components=2, random_state=0, n_iter=iterations, perplexity=20)

    # Fit the model to the features
    tsne_features = model.fit_transform(pure_features, y=labels)

    # Create a new dataframe with the t-SNE features
    tsne_df = pd.DataFrame(data=tsne_features, columns=['x', 'y'])

    # add the class names back to the dataframe
    tsne_df['ClassName'] = labels

    #randomly select 10 classes for which we plot all the points
    np.random.seed(0)
    random_classes = np.random.choice(class_names, 10)

    # fully select all samples for 10 random classes
    #tsne_df = tsne_df.loc[tsne_df['ClassName'].isin(random_classes)]

    tsne_df = tsne_df.loc[tsne_df['ClassName'].isin(["Bicycle", "Door", "Wheel", "Bus", "BuildingNonResidential"])]

    # Plot 10 random classes from the dataset with seaborn
    plt.figure(dpi=100, figsize=(9,6))
    sns.scatterplot(x='x', y='y', hue='ClassName', data=tsne_df)
    plt.savefig(f"../../res/t-SNE_plots/t-SNE_{iterations}.png")
