import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from tqdm import tqdm

# load in a feature file and store in a pandas dataframe
features = pd.read_csv("../../res/features_final.txt", sep='\t')

# exract labels
labels = features['ClassName']

# compute class frequencies
class_frequencies = []
class_names = np.unique(labels)
for class_name in class_names:
    class_frequencies.append(len(features[features['ClassName'] == class_name]))

class_indices = []
for class_name in class_names:
    # get all indices for the current class
    indices = features.index[features['ClassName'] == class_name].tolist()
    class_indices.append(indices)

# load in distance matrix
distance_matrix = pd.read_csv("../../res/distance_matrix/0_0_0.700000.txt", sep=',', header=None)

# drop the last empty column
distance_matrix = distance_matrix.drop(columns=distance_matrix.columns[-1])

# For each instance, sort the distances to all other instances storing the indices
sorted_indices = np.argsort(distance_matrix, axis=1)

print(sorted_indices)

# Compute K-tiers, so for each class compute the number of instances in each tier
k_tiers = [[] for _ in range(len(class_names))] 
for i in tqdm(range(len(class_names))):
    k_tier = np.zeros(10)
    
    # iterate over all instances of the class
    for inst in class_indices[i]:
        for k in range(10):
            k_tier[k] += len([x for x in sorted_indices[inst][:class_frequencies[i]*(k+1)] if labels[x] == class_names[i]]) / (class_frequencies[i]*(k+1) * class_frequencies[i]) 

    k_tiers[i] = k_tier
    
k_tiers = np.array(k_tiers)

# sort the list of k_tiers on the first tier in descending order, also keep the original indices
sorted_indices = np.argsort(k_tiers[:,0])[::-1]

print(k_tiers)

to_plot = [0, len(sorted_indices)//4, len(sorted_indices)//2, 3*len(sorted_indices)//4, len(sorted_indices)-1]

# # Plot the K-tiers
plt.figure(dpi=100)
for i in to_plot:
    plt.plot(k_tiers[sorted_indices[i]], label=class_names[sorted_indices[i]])
plt.legend()
plt.show()