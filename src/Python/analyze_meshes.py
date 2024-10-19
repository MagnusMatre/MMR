import pandas as pd
import os
import seaborn as sns

import matplotlib.pyplot as plt

file_path_okay = '../../data/OkayMeshes'
file_path_smallchange = '../../data/SmallchangeMeshes'
file_path_hopeless = '../../data/HopelessMeshes'


# Count the number of files in each subdirectory of the OkayMeshes directory
# This will give us the number of meshes in each class
def get_class_counts(file_path):
    class_counts= {}
    for class_name in os.listdir(file_path):
        class_dir = os.path.join(file_path, class_name)
        class_counts[class_name] = len(os.listdir(class_dir))

    # Create a DataFrame for plotting
    class_counts = pd.DataFrame({
        'Class': list(class_counts.keys()),
        'Count': list(class_counts.values())
    })

    class_counts = class_counts.sort_values(by='Count', ascending=False)

    return class_counts

# Sort the DataFrame by the number of meshes in each class
class_counts_okay = get_class_counts(file_path_okay)
class_counts_smallchange = get_class_counts(file_path_smallchange)
class_counts_hopeless = get_class_counts(file_path_hopeless)

# Plot the number of meshes in each class such that for each class the three numbers are shown 
# using different colors stacked on top of each other given the counts in class_counts_okay, 
# class_counts_smallchange, and class_counts_hopeless

# Create a DataFrame for plotting
plot_data = pd.DataFrame({
    'Okay': class_counts_okay['Count'],
    'Smallchange': class_counts_smallchange['Count'],
    'Hopeless': class_counts_hopeless['Count']
}).fillna(0)

# sort the data on the number of okay meshes
plot_data = plot_data.sort_values(by='Okay', ascending=True)

# Plot the bars
ax = plot_data.plot(kind='bar', stacked=True, figsize=(10, 6), color=['green', 'orange', 'red'])

# plot horizontal line at the minimum number of total mesh count, also show the number of meshes in the plot
min_total_meshes = plot_data.sum(axis=1).min()
ax.axhline(y=min_total_meshes, color='black', linestyle='--')
ax.set_title('Number of Meshes in Each Class')
ax.set_xlabel('Class Name')
ax.set_ylabel('Number of Meshes')
ax.set_xticklabels(class_counts_okay['Class'], rotation=90)
plt.tight_layout()
plt.show()




if False:
    # Read the data from the .txt file
    file_path = '../../data/VCGdecimatedCleaned/mesh_check.txt'
    data = pd.read_csv(file_path)

    save_dir = '../../res/mesh_processing_plots/'

    # count how many meshes have zero vertices
    print('Number of meshes with zero vertices:', (data['FaceCount'] == "0").sum())

    # count the number of meshes that have a number of vertices between 4000 and 4500
    print('Number of meshes with vertices between 4000 and 4500:', ((data['VertexCount'] >= 3500) & (data['VertexCount'] <= 5000)).sum())

    # count total number of meshes
    print('Total number of meshes:', data.shape[0])

    # print percentage of meshes that contain between 4000 and 4500 vertices
    print('Percentage of meshes with vertices between 4000 and 4500:', ((data['VertexCount'] >= 3500) & (data['VertexCount'] <= 5000)).sum() / data.shape[0])

if False:
    # Plot 1: Boxplot for the number of vertices per class with dots
    plt.figure(figsize=(10, 6))
    sns.boxplot(x='Class', y='VertexCount', data=data, showfliers=False)
    sns.stripplot(x='Class', y='VertexCount', data=data, color='black', size=3, jitter=True)
    #plt.title('Number of Vertices per Class')
    plt.xlabel('Class Name')
    plt.ylabel('Number of Vertices')
    plt.xticks(rotation=90)
    plt.tight_layout()
    plt.savefig(save_dir + 'vertices_per_class.png')
    plt.show()

    # Plot 2: Boxplot for the number of faces per class with dots
    plt.figure(figsize=(10, 6))
    sns.boxplot(x='Class', y='FaceCount', data=data, showfliers=False)
    sns.stripplot(x='Class', y='FaceCount', data=data, color='black', size=3, jitter=True)
    #plt.title('Number of Faces per Class')
    plt.xlabel('Class Name')
    plt.ylabel('Number of Faces')
    plt.xticks(rotation=90)
    plt.tight_layout()
    plt.savefig(save_dir + 'faces_per_class.png')
    plt.show()

    # Plot 3: Number of meshes in each class with different colors for closed and total meshes
    plt.figure(figsize=(10, 6))

    # Calculate the number of closed meshes and total meshes
    class_counts_total = data['Class'].value_counts()

    # Calculate the number of closed meshes, the closed column is a "0" and "1" column, so we can use it directly
    class_counts_closed = data.groupby('Class')['Closed'].sum()
    print(class_counts_closed)

    # Create a DataFrame for plotting
    plot_data = pd.DataFrame({
        'Total': class_counts_total,
        'Closed': class_counts_closed
    }).fillna(0)

    # Plot the bars
    plot_data.plot(kind='bar', color=['blue', 'red'])

    plt.title('Number of Meshes in Each Class')
    plt.xlabel('Class Name')
    plt.ylabel('Number of Meshes')
    plt.xticks(rotation=90)
    plt.tight_layout()
    #plt.savefig(save_dir + 'meshes_per_class.png')
    #plt.show()

    # Plot 4: Boxplot for the ratio between the number of faces and vertices for each class
    data['FacesToVerticesRatio'] = data['FaceCount'] / data['VertexCount']
    plt.figure(figsize=(10, 6))
    sns.boxplot(x='Class', y='FacesToVerticesRatio', data=data)
    plt.title('Ratio of Faces to Vertices per Class')
    plt.xlabel('Class Name')
    plt.ylabel('Faces to Vertices Ratio')
    plt.xticks(rotation=90)
    plt.tight_layout()
    #plt.savefig(save_dir + 'faces_to_vertices_ratio_per_class.png')
    #plt.show()