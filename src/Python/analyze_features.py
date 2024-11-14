import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

import os
import sys
from tqdm import tqdm

TEST_PRIMITIVES = False

feature = None
if TEST_PRIMITIVES:
    features = pd.read_csv("../../res/features_primitives.txt", sep='\t')
    features['Compactness'] = np.exp(features['Compactness'])
    features['Sphericity'] = np.exp(features['Sphericity'])

    print(features.iloc[:, [0, 2, 5, 6, 7, 8, 9, 10, 11, 12, 13, 20]])


if not TEST_PRIMITIVES:
    features = pd.read_csv("../../res/features_final.txt", sep='\t')

    # for instance from class "Bottle" with filename "D00696.obj", print the features
    for i in range(features.shape[0]):
        if features.iloc[i, 0] == "Bottle" and features.iloc[i, 1] == "D00696.obj":
            print(features.iloc[i, :20])
    
    features['Compactness'] = np.exp(features['Compactness'])
    features['Sphericity'] = np.exp(features['Sphericity'])

    # count number of shapes with convexity value larger than 1
    print(np.sum(features['Convexity'] > 1))

    for i in range(2, 21):
        min_index = np.argmin(features.iloc[:, i])
        max_index = np.argmax(features.iloc[:, i])
        print(f"(min, max) for {features.columns[i]} is ({np.min(features.iloc[:, i])}, {np.max(features.iloc[:, i])}) - min for ({features.iloc[min_index, 0]},{features.iloc[min_index, 1]}) - max for ({features.iloc[max_index, 0]},{features.iloc[max_index, 1]})")

    features['Compactness'] = np.log(features['Compactness'])
    features['Sphericity'] = np.log(features['Sphericity'])
    

print(features.shape)

# print a list with the number of instances per class sorted in alphabetical order
# print(list(x for x in features['ClassName'].value_counts().sort_index()))
# print(len(list(x for x in features['ClassName'].value_counts().sort_index())))
# print(sum(list(x for x in features['ClassName'].value_counts().sort_index())))
#quit()
#print(features.iloc[2284, :])
#print(features)

# remove the outlier features
#features = features.drop(outlier_indices)

# compute only for columns 2 to 18
features_to_avg = features.iloc[:, [0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]]

mean_features = features_to_avg .groupby('ClassName').mean()
std_features = features_to_avg .groupby('ClassName').std()

class_names = mean_features.index

# check the number of occurernces where a feature attains a value <0
# for i in range(2, 21):
#     print(f"Feature {features.columns[i]}: {np.sum(features.iloc[:, i] < 0)}")
# what is column 11?

# Set to true to plot scalar features
if False:
    for i in range(0,19):
        # copy the list of class name together with the mean value for feature i
        # get the name of feature i
        feature_name = mean_features.columns[i]
        class_mean = mean_features.iloc[:, i]
        class_mean = class_mean.reset_index()
        class_mean.columns = ['Class Name', 'Mean Value']
        class_mean['Class Name'] = class_names
        class_mean = class_mean.sort_values(by='Mean Value')
        plt.figure(figsize=(10,6), dpi=300)
        plt.errorbar(class_mean['Class Name'], class_mean['Mean Value'], yerr=std_features.iloc[:, i], fmt='o', color='black', ecolor='blue')

        #plt.bar(class_mean['Class Name'], class_mean['Mean Value'])
        plt.xlabel("Class Name")
        if feature_name == "Sphericity" or feature_name == "Compactness":
            plt.ylabel("Mean value of logarithm of " + feature_name)
        elif feature_name == "Eccentricity02": #  smallest / largest
            plt.ylabel(r'Mean value of $E_{20}$')
        elif feature_name == "Eccentricity01": # smallest / middle
            plt.ylabel(r'Mean value of $E_{21}$')
        elif feature_name == "Eccentricity12": # middle / largest
            plt.ylabel(r'Mean value of $E_{10}$')
        else:
            plt.ylabel("Mean value of " + feature_name)
        plt.xticks(rotation=90)
        plt.tight_layout()
        plt.savefig(f"../../res/feature_plots_final/OkayMeshes3/mean_{feature_name}.png")
        #plt.show()



#plot the mean values of the scalar features for each class
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

# Extract the histogram features of A3
histogram_A3 = features.iloc[:, 21:121]
histogram_D1 = features.iloc[:, 121:221]
histogram_D2 = features.iloc[:, 221:321]
histogram_D3 = features.iloc[:, 321:421]
histogram_D4 = features.iloc[:, 421:521]

#compute the mean histogram per class
mean_histogram_A3 = histogram_A3.groupby(features['ClassName']).mean()
mean_histogram_D1 = histogram_D1.groupby(features['ClassName']).mean()
mean_histogram_D2 = histogram_D2.groupby(features['ClassName']).mean()
mean_histogram_D3 = histogram_D3.groupby(features['ClassName']).mean()
mean_histogram_D4 = histogram_D4.groupby(features['ClassName']).mean()


if TEST_PRIMITIVES:
    # for each primitive class, Cube, Sphere, Cylinder, Cone, plot all histograms in a 5x4 grid with the 5 histograms
    # in each column belonging to the same shape
    fig, axes = plt.subplots(5, 4, sharex='row', figsize=(13,20))

    # set the label of rows 
    axes[0, 0].set_ylabel("A3")
    axes[1, 0].set_ylabel("D1")
    axes[2, 0].set_ylabel("D2")
    axes[3, 0].set_ylabel("D3")
    axes[4, 0].set_ylabel("D4")

    # set the title of each subplot to the name of the shape
    axes[0, 0].set_title("Cone")
    axes[0, 1].set_title("Cube")
    axes[0, 2].set_title("Cylinder")
    axes[0, 3].set_title("Sphere")

    # plot the histograms of A3 for each class
    for j in range(4):
        axes[0, j].plot(histogram_A3.iloc[j, :], color='blue')
        axes[0, j].set_xticks(np.linspace(0, 100, 5))
        axes[0, j].set_xticklabels(["0", "π/4", "π/2", "3π/4", "π"], fontsize=8)
        axes[0, j].set_xlim(0, 100)

    for j in range(4):
        axes[1, j].plot(histogram_D1.iloc[j, :50], color='blue')
        axes[1, j].set_xticks(np.linspace(0, 50, 5))
        axes[1, j].set_xticklabels(["0", r'$\frac{1}{8}\sqrt{3}$', r'$\frac{1}{4}\sqrt{3}$', r'$\frac{3}{8}\sqrt{3}$', r'$\frac{1}{2}\sqrt{3}$'], fontsize=8)
        axes[1, j].set_xlim(0, 50)
        
    for j in range(4):
        axes[2, j].plot(histogram_D2.iloc[j, :], color='blue')
        axes[2, j].set_xticks(np.linspace(0, 100, 5))
        axes[2, j].set_xticklabels(["0", r'$\frac{1}{4}\sqrt{3}$', r'$\frac{1}{2}\sqrt{3}$', r'$\frac{3}{4}\sqrt{3}$', r'$\sqrt{3}$'], fontsize=8)
        axes[2, j].set_xlim(0, 100)

    for j in range(4):
        axes[3, j].plot(histogram_D3.iloc[j, :], color='blue')
        axes[3, j].set_xticks(np.linspace(0, 100, 5))
        axes[3, j].set_xticklabels(["0", r'$\frac{1}{4}\sqrt{\sqrt{3}}$', r'$\frac{1}{2}\sqrt{\sqrt{3}}$', r'$\frac{3}{4}\sqrt{\sqrt{3}}$', r'$\sqrt{\sqrt{3}}$'], fontsize=8)
        axes[3, j].set_xlim(0, 100)

    for j in range(4):
        axes[4, j].plot(histogram_D4.iloc[j, :], color='blue')
        axes[4, j].set_xticks(np.linspace(0, 100, 5))
        axes[4, j].set_xticklabels(["0", r'$\frac{1}{4}\sqrt[3]{\frac{1}{3}}$', r'$\frac{1}{2}\sqrt[3]{\frac{1}{3}}$', r'$\frac{3}{4}\sqrt[3]{\frac{1}{3}}$', r'$\sqrt[3]{\frac{1}{3}}$'], fontsize=8)
        axes[4, j].set_xlim(0, 100)

    plt.tight_layout(rect=[0, 0.05, 1, 0.95])

    # add a bit of margin between the rows
    plt.subplots_adjust(hspace=0.4)

    plt.show()
            

if not TEST_PRIMITIVES:
    # Randomly select 16 classes from the features dataframe
    np.random.seed(1)
    selected_classes = np.random.choice(features['ClassName'].unique(), 16, replace=False)

    # for each class, randomly select one instance and print the class name and filename
    # for class_name in selected_classes:
    #     class_features = features[features['ClassName'] == class_name]
    #     random_instance = class_features.sample()
    #     print(random_instance[['ClassName', 'Filename']])


    # for each class, randomly select one instance and print only its index
    random_instances = []
    for class_name in features['ClassName'].unique():
        class_features = features[features['ClassName'] == class_name]
        random_instance = class_features.sample(5)
        random_instances.extend(list(random_instance.index.values))

    #print("{", f"{[x for x in random_instances]}", "}")

    A3 = 0
    D1 = 0
    D2 = 0
    D3 = 0
    D4 = 0

    # Check if any histograms of D1 has a non-zero value to the right of the 50th bin
    # for i in range(histogram_D1.shape[0]):
    #     if np.sum(histogram_D1.iloc[i, 57:]) > 0:
    #         print(f"Non-zero value in D1 at index {i}")

    # print the class name and filename of index 1777
    #print(features.iloc[1777, [0, 1]])


    if A3:
    #For each of the 16 selected classes, plot the histograms A3 in a 4x4 grid
        fig, axes = plt.subplots(4, 4, sharex='row', figsize=(12,12))
        fig.suptitle("Histograms of A3", fontsize=16)
        for i in range(16):
            axes[i//4, i%4].set_title(selected_classes[i], fontsize=10)
            axes[i//4, i%4].set_xlim(0, 100)
            # plot the all the histograms belonging to the class selected_classes[i]
            for j in range(histogram_A3.shape[0]):
                if features.iloc[j, 0] == selected_classes[i]:
                    axes[i//4, i%4].plot(histogram_A3.iloc[j, :], color='blue', alpha=0.15)
            
            # plot the mean histogram for the specified class name in black
            axes[i//4, i%4].plot(mean_histogram_A3.loc[selected_classes[i], :], color='black', linewidth=1)
            
            # dont plot any text on the x-axis
            axes[i//4, i%4].set_xticks([])

            # for the bottom row, plot the x-axis
            if i//4 == 3:
                # transform the ticks such that there are 5 ticks from 0 to 3.1415
                axes[i//4, i%4].set_xticks(np.linspace(0, 100, 5))
                axes[i//4, i%4].set_xticklabels(["0", "π/4", "π/2", "3π/4", "π"])
                axes[i//4, i%4].set_xlabel("Angle (radians)")

            plt.tight_layout(rect=[0, 0.05, 1, 0.95])

            # add a bit of margin between the rows
            plt.subplots_adjust(hspace=0.2)
        plt.savefig("../../res/feature_plots_final/OkayMeshes3/histogram_A3.png")
        #plt.show()

    if D1:
    #For each of the 16 selected classes, plot the histograms A3 in a 4x4 grid
        fig, axes = plt.subplots(4, 4, sharex='row', figsize=(12,12))
        fig.suptitle("Histograms of D1", fontsize=16)
        for i in range(16):
            axes[i//4, i%4].set_title(selected_classes[i], fontsize=10)
            axes[i//4, i%4].set_xlim(0, 50)
            # plot the all the histograms belonging to the class selected_classes[i]
            for j in range(histogram_D1.shape[0]):
                if features.iloc[j, 0] == selected_classes[i]:
                    axes[i//4, i%4].plot(histogram_D1.iloc[j, :50], color='blue', alpha=0.15)
            
            # plot the mean histogram for the specified class name in black
            axes[i//4, i%4].plot(mean_histogram_D1.loc[selected_classes[i], :], color='black', linewidth=1)

            # dont plot any text on the x-axis
            axes[i//4, i%4].set_xticks([])

            # for the bottom row, plot the x-axis
            if i//4 == 3:
                # transform the ticks such that there are 5 ticks from 0 to 3.1415
                axes[i//4, i%4].set_xticks(np.linspace(0, 50, 5))
                axes[i//4, i%4].set_xticklabels(["0", r'$\frac{1}{8}\sqrt{3}$', r'$\frac{1}{4}\sqrt{3}$', r'$\frac{3}{8}\sqrt{3}$', r'$\frac{1}{2}\sqrt{3}$'])
                axes[i//4, i%4].set_xlabel("Distance")

            plt.tight_layout(rect=[0, 0.05, 1, 0.95])

            # add a bit of margin between the rows
            plt.subplots_adjust(hspace=0.2)
        #plt.show()
        plt.savefig("../../res/feature_plots_final/OkayMeshes3/histogram_D1.png")

    if D2:
    #For each of the 16 selected classes, plot the histograms A3 in a 4x4 grid
        fig, axes = plt.subplots(4, 4, sharex='row', figsize=(12,12))
        fig.suptitle("Histograms of D2", fontsize=16)
        for i in range(16):
            axes[i//4, i%4].set_title(selected_classes[i], fontsize=10)
            axes[i//4, i%4].set_xlim(0, 100)
            # plot the all the histograms belonging to the class selected_classes[i]
            for j in range(histogram_D2.shape[0]):
                if features.iloc[j, 0] == selected_classes[i]:
                    axes[i//4, i%4].plot(histogram_D2.iloc[j, :], color='blue', alpha=0.15)
            
            # plot the mean histogram for the specified class name in black
            axes[i//4, i%4].plot(mean_histogram_D2.loc[selected_classes[i], :], color='black', linewidth=1)

            # dont plot any text on the x-axis
            axes[i//4, i%4].set_xticks([])

            # for the bottom row, plot the x-axis
            if i//4 == 3:
                # transform the ticks such that there are 5 ticks from 0 to 3.1415
                axes[i//4, i%4].set_xticks(np.linspace(0, 100, 5))
                axes[i//4, i%4].set_xticklabels(["0", r'$\frac{1}{4}\sqrt{3}$', r'$\frac{1}{2}\sqrt{3}$', r'$\frac{3}{4}\sqrt{3}$', r'$\sqrt{3}$'])
                axes[i//4, i%4].set_xlabel("Distance")

            plt.tight_layout(rect=[0, 0.05, 1, 0.95])

            # add a bit of margin between the rows
            plt.subplots_adjust(hspace=0.2)
        #plt.show()
        plt.savefig("../../res/feature_plots_final/OkayMeshes3/histogram_D2.png")

    if D3:
    #For each of the 16 selected classes, plot the histograms A3 in a 4x4 grid
        fig, axes = plt.subplots(4, 4, sharex='row', figsize=(12,12))
        fig.suptitle("Histograms of D3", fontsize=16)
        for i in range(16):
            axes[i//4, i%4].set_title(selected_classes[i], fontsize=10)
            axes[i//4, i%4].set_xlim(0, 100)
            # plot the all the histograms belonging to the class selected_classes[i]
            for j in range(histogram_D3.shape[0]):
                if features.iloc[j, 0] == selected_classes[i]:
                    axes[i//4, i%4].plot(histogram_D3.iloc[j, :], color='blue', alpha=0.15)
            
            # plot the mean histogram for the specified class name in black
            axes[i//4, i%4].plot(mean_histogram_D3.loc[selected_classes[i], :], color='black', linewidth=1)

            # dont plot any text on the x-axis
            axes[i//4, i%4].set_xticks([])

            # for the bottom row, plot the x-axis
            if i//4 == 3:
                # transform the ticks such that there are 5 ticks from 0 to 3.1415
                axes[i//4, i%4].set_xticks(np.linspace(0, 100, 5))
                axes[i//4, i%4].set_xticklabels(["0", r'$\frac{1}{4}\sqrt{\sqrt{3}}$', r'$\frac{1}{2}\sqrt{\sqrt{3}}$', r'$\frac{3}{4}\sqrt{\sqrt{3}}$', r'$\sqrt{\sqrt{3}}$'])
                axes[i//4, i%4].set_xlabel("Square root area")

            plt.tight_layout(rect=[0, 0.05, 1, 0.95])

            # add a bit of margin between the rows
            plt.subplots_adjust(hspace=0.2)
        #plt.show()
        plt.savefig("../../res/feature_plots_final/OkayMeshes3/histogram_D3.png")

    if D4:
    #For each of the 16 selected classes, plot the histograms A3 in a 4x4 grid
        fig, axes = plt.subplots(4, 4, sharex='row', figsize=(12,12))
        fig.suptitle("Histograms of D4", fontsize=16)
        for i in range(16):
            axes[i//4, i%4].set_title(selected_classes[i], fontsize=10)
            axes[i//4, i%4].set_xlim(0, 100)
            # plot the all the histograms belonging to the class selected_classes[i]
            for j in range(histogram_D4.shape[0]):
                if features.iloc[j, 0] == selected_classes[i]:
                    axes[i//4, i%4].plot(histogram_D4.iloc[j, :], color='blue', alpha=0.15)
            
            # plot the mean histogram for the specified class name in black
            axes[i//4, i%4].plot(mean_histogram_D4.loc[selected_classes[i], :], color='black', linewidth=1)

            # dont plot any text on the x-axis
            axes[i//4, i%4].set_xticks([])

            # for the bottom row, plot the x-axis
            if i//4 == 3:
                # transform the ticks such that there are 5 ticks from 0 to 3.1415
                axes[i//4, i%4].set_xticks(np.linspace(0, 100, 5))
                axes[i//4, i%4].set_xticklabels(["0", r'$\frac{1}{4}\sqrt[3]{\frac{1}{3}}$', r'$\frac{1}{2}\sqrt[3]{\frac{1}{3}}$', r'$\frac{3}{4}\sqrt[3]{\frac{1}{3}}$', r'$\sqrt[3]{\frac{1}{3}}$'])
                axes[i//4, i%4].set_xlabel("Cube root volume")

            plt.tight_layout(rect=[0, 0.05, 1, 0.95])

            # add a bit of margin between the rows
            plt.subplots_adjust(hspace=0.2)
        #plt.show()
        plt.savefig("../../res/feature_plots_final/OkayMeshes3/histogram_D4.png")