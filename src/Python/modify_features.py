import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

import os
import sys
from tqdm import tqdm

'''
    - Modify the raw feature files to be in the correct format:
        - Shift columns
        - Add secondary features

'''

# load in a feature file and store in a pandas dataframe
#feature_dir = "../../data/OkayMeshes3"
feature_dir = "../../data/PrimitivesNormalized"

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
    if class_name == "features_large.txt":
        continue
    feature_path = os.path.join(feature_dir, class_name, "features_large.txt")
    
    feature_list.append(load_feauture_file(feature_path))

features = stitch_together_feature_files(feature_list)

# replace all values X in the column named Eccentricity by sqrt(1-X^2) -- LOOKS WAY BETTER
features['Eccentricity02'] = np.sqrt(1 - features['Eccentricity02']**2)
features['Eccentricity01'] = np.sqrt(1 - features['Eccentricity01']**2)
features['Eccentricity12'] = np.sqrt(1 - features['Eccentricity12']**2)

# take the log of compactness and sphericity
features['Compactness'] = np.log(features['Compactness'])
features['Sphericity'] = np.log(features['Sphericity'])

#Insert new column at index 14 named "CubeRootVolume" which is the cube root of the Volume column
features.insert(14, "CubeRootVolume", np.cbrt(features['Volume']))

#Insert new column at index 15 named "CubeRootVolume" which is the cube root of the Volume column
features.insert(15, "SquareRootArea", np.cbrt(features['SurfaceArea']))

#Insert new column at index 16 named DiameterToCubeRootVolume which is the Diameter divided by the cube root of the Volume
features.insert(16, "DiameterToCubeRootVolume", features['Diameter'] / features['CubeRootVolume'])

#Insert new column at index 17 named DiameterToCubeRootVolume which is the Diameter divided by the cube root of the Volume
features.insert(17, "DiameterToSquareRootArea", features['Diameter'] / features['SquareRootArea'])

#Insert new column at index 18 named DiameterSquared which is the Diameter squared
features.insert(18, "DiameterSquared", features['Diameter']**2)

# Insert new column at index 19 named BBVolumeCubeRoot which is the cube root of the bounding box volume
features.insert(19, "BBVolumeCubeRoot", np.cbrt(features['BB_Volume']))

print(features.columns)

# save the features to a new file
features.to_csv("../../res/features_primitives.txt", sep='\t', index=False)