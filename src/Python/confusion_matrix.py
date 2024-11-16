import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from tqdm import tqdm

CONFUSION_MATRIX = False

# load in a feature file and store in a pandas dataframe
features = pd.read_csv("../../res/features_final.txt", sep='\t')

# create lists that contain all the instance indices for each class
class_indices = []

# get the unique class names
class_names = np.unique(features['ClassName'])

# iterate through all class names
for class_name in class_names:
    # get all indices for the current class
    indices = features.index[features['ClassName'] == class_name].tolist()
    class_indices.append(indices)

# load in distance matrix
distance_matrix = pd.read_csv("../../res/distance_matrix/0_0_0.700000.txt", sep=',', header=None)

# drop the last empty column
distance_matrix = distance_matrix.drop(columns=distance_matrix.columns[-1])

print(distance_matrix)

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


if CONFUSION_MATRIX == True:
    class_distances = np.zeros((69,69))

    # for i in tqdm(range(69)):
    #     for j in range(i, 69):
    #         class_distance_i_to_j = 0

    #         # iterate through all instances of class i
    #         for ii in class_indices[i]:
    #             # iterate through all instances of class j
    #             for jj in class_indices[j]:
    #                 class_distance_i_to_j += distance_matrix.iloc[ii, jj]
            
    #         class_distance_i_to_j /= len(class_indices[i]) * len(class_indices[j])
    #         class_distances[i, j] = class_distance_i_to_j
    #         class_distances[j, i] = class_distance_i_to_j

    # np.savetxt("../../res/distance_matrix/class_distances.txt", class_distances, delimiter=',')

    class_distances = np.loadtxt("../../res/distance_matrix/class_distances.txt", delimiter=',')

    unique_labels = np.unique(labels)

    # Create the a confusion matrix with seaborn using the distance_matrix as input
    plt.figure(dpi=100, figsize=(10, 10))
    sns.heatmap(class_distances, cmap="bone", xticklabels=unique_labels, yticklabels=unique_labels)
    plt.tight_layout()
    plt.show()