import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

import os
import sys
from tqdm import tqdm

# load in a feature file and store in a pandas dataframe
feature_dir = "../../data/OkayMeshes"

def load_feauture_file(feature_file):
    # Read the data from the .txt file, note that the file is tab separated
    features = pd.read_csv(feature_file, sep='\t')

    # shift every column two places to the right starting at index 4
    for i in range(features.shape[1]-1, 3, -1):
        features.iloc[:, i] = features.iloc[:, i-2]

    # replace the column at index 2 with the column at index 1
    features.iloc[:, 2] = features.iloc[:, 1]

    #drop column at index 1 and 3
    features = features.drop(features.columns[1], axis=1)
    features = features.drop(features.columns[2], axis=1)

    return features

def stitch_together_feature_files(feature_list):
    # concatenate all the feature files
    return pd.concat(feature_list)


feature_list = []
for class_name in tqdm(os.listdir(feature_dir)):
    feature_path = os.path.join(feature_dir, class_name, "features.txt")
    
    feature_list.append(load_feauture_file(feature_path))

features = stitch_together_feature_files(feature_list)

print(features)

features = features[features['Volume'] < 8]
features = features[features['BB_Volume'] < 8]

features = features.drop(features.columns[1], axis=1)

# Plot the mean value of the scalar features for each class with each class on the x-axis
mean_features = features.groupby('ClassName').mean()
std_features = features.groupby('ClassName').std()

# if the mean corresponding to feature Volume is larger than 8 then discard the class

class_names = mean_features.index

plt.figure()
for i in range(0,13):
    # copy the list of class name together with the mean value for feature i
    # get the name of feature i
    feature_name = mean_features.columns[i]
    class_mean = mean_features.iloc[:, i]
    class_mean = class_mean.reset_index()
    class_mean.columns = ['Class Name', 'Mean Value']
    class_mean['Class Name'] = class_names
    class_mean = class_mean.sort_values(by='Mean Value')
    plt.figure(figsize=(10,6))
    #plt.errorbar(class_mean['Class Name'], class_mean['Mean Value'], yerr=std_features.iloc[:, i], fmt='o')

    plt.bar(class_mean['Class Name'], class_mean['Mean Value'])
    plt.xlabel("Class Name")
    plt.ylabel("Mean value of " + feature_name)
    plt.xticks(rotation=90)
    plt.tight_layout()
    plt.show()


# plot the mean values of the scalar features for each class
# plt.figure()
# for i in range(mean_features.shape[0]):
#     plt.plot(mean_features.iloc[i, 1:13])
#     plt.xlabel("Feature Index")
#     plt.ylabel("Mean Value")
#     plt.legend(mean_features.index)
# plt.show()


# the first two columns ClassName and Filename are have to delimiters after them, but the rest of the columns are tab separated


# replace the column at index 3 with the column at index 2
#features.iloc[:, 3] = features.iloc[:, 2]

# The columns 2-14 correspond to scalar features
# The columns 15-74 correspond to histogram features of A3
# The columns 75-178 correspond to histogram features of D1
# The columns 179-252 correspond to histogram features of D2
# The columns 253-310 correspond to histogram features of D3
# The columns 311-361 correspond to histogram features of D4

# Extract the histogram features of A3
# histogram_A3 = features.iloc[:, 15:75]
# histogram_D1 = features.iloc[:, 75:179]
# histogram_D2 = features.iloc[:, 179:253]
# histogram_D3 = features.iloc[:, 253:312]
# histogram_D4 = features.iloc[:, 313:365]

# For each row, plot the histoogram using the column values as the bins
# plt.figure()
# for i in range(histogram_A3.shape[0]):
#     plt.plot(histogram_A3.iloc[i, :])
#     plt.xlabel("Angle (degrees)")
#     # set the xticks to go from 0 to pi
#     plt.xticks(np.arange(0, 61, 10), np.arange(0, 181, 30))
# plt.show()

#print(histogram_D1)

# plt.figure()
# for i in range(histogram_D1.shape[0]):
#     plt.plot(histogram_D1.iloc[i, :])
#     plt.xlabel("Distance 1")
#     # set the xticks to go from 0 to 2sqrt(3)
#     plt.xticks(np.arange(0, 61, 10), np.arange(0, 3.1, 0.5))
# plt.show()

# print(histogram_D2)

# plt.figure()
# for i in range(histogram_D2.shape[0]):
#     plt.plot(histogram_D2.iloc[i, :])
#     plt.xlabel("Distance 2")
#     # set the xticks to go from 0 to 2sqrt(3)
#     plt.xticks(np.arange(0, 61, 10), np.arange(0, 3.1, 0.5))
# plt.show()

# print(histogram_D4)

# plt.figure()
# for i in range(histogram_D4.shape[0]):
#     plt.plot(histogram_D4.iloc[i, :])
#     plt.xlabel("Distance 4")
#     # set the xticks to go from 0 to 2sqrt(3)
#     plt.xticks(np.arange(0, 61, 10), np.arange(0, 1.3, 0.2))
# plt.show()