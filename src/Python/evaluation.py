import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from tqdm import tqdm

def compute_AUC(sens_spec):
    # compute the area under the curve
    auc = 0
    for i in range(1, len(sens_spec)):
        auc += (sens_spec[i][0] - sens_spec[i-1][0]) * (sens_spec[i][1] + sens_spec[i-1][1]) / 2
    return auc

# load in a feature file and store in a pandas dataframe
features = pd.read_csv("../../res/features_final.txt", sep='\t')

# exract labels
labels = features['ClassName']

# compute class frequencies
class_frequencies = []
class_names = np.unique(labels)
for class_name in class_names:
    class_frequencies.append(len(features[features['ClassName'] == class_name]))

# create dictionary that maps class names to their frequencies
class_frequencies_map = {}
for i in range(len(class_names)):
    class_frequencies_map[class_names[i]] = len(features[features['ClassName'] == class_names[i]])

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

#print(sorted_indices)

# delete the first column, as it is the index of the instance itself
sorted_indices = np.delete(sorted_indices, 0, axis=1) 

TP_matrix = np.zeros((len(features), len(features)))

# iterate over all instances
# for i in tqdm(range(len(features))):
#     for q in range(len(features)-1):
#         if q == 0:
#             if labels[sorted_indices[i][q]] == labels[i]:
#                 TP_matrix[i][q] = 1
#             else:
#                 TP_matrix[i][q] = 0
#         else:
#             if labels[sorted_indices[i][q]] == labels[i]:
#                 TP_matrix[i][q] = TP_matrix[i][q-1] + 1
#             else:
#                 TP_matrix[i][q] = TP_matrix[i][q-1]

#np.savetxt("../../res/TP_matrix/matrix.txt", TP_matrix)
TP_matrix = np.loadtxt("../../res/TP_matrix/matrix.txt")
print(TP_matrix)
# careful when subtracting 1 from class_frequencies[i] when you don't want to count the query instance itself

K_TIERS = True
PRECISION = False # this probably produces the "best" results in the sense that it looks the best for us
LAST_RANK = False
ACCURACY = False # useless
F1_SCORE = False 
ROC_CURVE = False # this one is interesting

selected_classes = ["Bicycle", "Door", "Wheel", "Bus", "BuildingNonResidential"]

if K_TIERS:
    # Compute K-tiers, so for each class compute the number of instances in each tier
    k_tiers = [[] for _ in range(len(class_names))] 
    for i in tqdm(range(len(class_names))):
        k_tier = np.zeros(10)
        
        # iterate over all instances of the class
        for inst in class_indices[i]:
            for k in range(10):
                k_tier[k] += len([x for x in sorted_indices[inst][(class_frequencies[i]-1)*(k):(class_frequencies[i]-1)*(k+1)] if labels[x] == class_names[i]]) / ((class_frequencies[i]-1) * class_frequencies[i]) 

        k_tiers[i] = k_tier

    k_tiers = np.array(k_tiers)

    # sort the list of k_tiers on the first tier in descending order, also keep the original indices
    sorted_indices = np.argsort(k_tiers[:,0])[::-1]

    print(k_tiers)

    to_plot = [0, len(sorted_indices)//4, len(sorted_indices)//2, 3*len(sorted_indices)//4, len(sorted_indices)-1]

    # # Plot the K-tiers
    plt.figure(figsize =(8,5))
    plt.title("K-tiers of some classes")
    for i in to_plot:
        plt.plot(k_tiers[sorted_indices[i]][:5], label=class_names[sorted_indices[i]])


    # change x-ticks starting from index 1
    plt.xticks(np.arange(5), np.arange(1, 6))
    plt.xlim(0,4)
    plt.ylim(0,1)
    plt.ylabel("Fraction of instances in tier")
    plt.xlabel("Kth tier")
    plt.legend()
    plt.show()

if PRECISION:
    # compute average precision per class, i.e., the number of times the first instance of the class is from the same class
    average_precision = np.zeros(len(class_names))
    total_average_precision = 0

    K = 10
    for i in tqdm(range(len(class_names))):
        class_prec = 0
        # iterate over all instances of the class
        for inst in class_indices[i]:
            # find the first instance of the class in the sorted indices
            for k in range(K):
                if sorted_indices[inst][k] in class_indices[i]:
                    class_prec += 1/10
                    total_average_precision += 1/10

        average_precision[i] = class_prec / class_frequencies[i]

    print(sum(average_precision) / len(class_names))
    print(total_average_precision / len(features))

    

    # plot the average precision in a bar chart per class
    plt.figure(dpi=100)
    plt.title(f"Average {K}-hit precision per class")
    plt.bar(class_names, average_precision)
    plt.xlabel("Class")
    plt.xticks(rotation=90)
    plt.tight_layout()
    plt.show()

if LAST_RANK:
    # Find the number of instances that are of the same rank until we hit a different class
    last_rank = np.zeros(len(class_names))
    total_last_rank = 0
    for i in tqdm(range(len(class_names))):
        class_last_rank = 0
        # iterate over all instances of the class
        for inst in class_indices[i]:
            cur_last_rank = 0
            # find the first instance of the class in the sorted indices
            for j in range(len(sorted_indices[inst])):
                if labels[sorted_indices[inst][j]] == class_names[i]:
                    cur_last_rank += 1
                else:
                    break

            class_last_rank += cur_last_rank / class_frequencies[i]

        last_rank[i] = class_last_rank
        total_last_rank += class_last_rank / (len(features) - 1)

    print(sum(last_rank) / len(class_names))
    print(total_last_rank / len(features))
    # plot the last rank in a bar chart per class
    plt.figure(dpi=100)
    plt.title("Last rank")
    plt.bar(class_names, last_rank, alpha=0.5, color="b")
    plt.xlabel("Class")
    plt.xticks(rotation=90)
    plt.tight_layout()
    plt.show()

if ACCURACY: # feels kind of useless because it focusses on true negatives...
    # Compute the accuracy with query size Q, so number of true positives + true negatives / number of instances
    Q_values = np.geomspace(1, len(features)-1, num=20, dtype=int)
    print(Q_values)
    total_accuracies = np.zeros(len(Q_values))
    class_accuracies = np.zeros((len(class_names), len(Q_values)))

    for Q, Qval in enumerate(Q_values):
        for i in tqdm(range(len(class_names))):
            true_postiive = 0
            # iterate over all instances of the class
            for inst in class_indices[i]:
                true_postiive = 0
                # find the first Q instances of the class in the sorted indices
                for j in range(Qval):
                    if labels[sorted_indices[inst][j]] == class_names[i]: # True positives
                        true_postiive += 1
                    
                # number of objects not retrieved (false negatives)
                false_negatives = (class_frequencies[i] - 1 - true_postiive)

                true_negatives = (len(features) - 1 - Qval) - false_negatives

                cur_accuracy = (true_postiive + true_negatives) / (len(features)-1)
                class_accuracies[i][Q] = cur_accuracy / class_frequencies[i]
                total_accuracies[Q] += cur_accuracy / len(features)

    # plot the total accuracies
    plt.figure(dpi=100)
    plt.title("Accuracy")
    plt.plot(Q_values, total_accuracies)
    plt.xlabel("Q")
    plt.xscale("log")
    plt.ylabel("Accuracy")
    plt.show()

    # plot the class accuracies for the select classes
    plt.figure(dpi=100)
    plt.title("Accuracy for selected classes")
    for i in range(len(class_names)):
        if class_names[i] in selected_classes:
            plt.plot(Q_values, class_accuracies[i], label=class_names[i])
    plt.legend()
    plt.xlabel("Q")
    plt.xscale("log")
    plt.ylabel("Accuracy")
    plt.show()


if F1_SCORE:
    # Compute the F1 score with query size Q, so 2 * (precision * recall) / (precision + recall)
    Q_values = list(range(1, 100, 3))
    print(Q_values)
    total_f1_scores_05 = np.zeros(len(Q_values))
    total_f1_scores_1 = np.zeros(len(Q_values))
    total_f1_scores_2 = np.zeros(len(Q_values))
    total_f1_scores_3 = np.zeros(len(Q_values))

    class_f1_scores = np.zeros((len(class_names), len(Q_values)))

    for Q, Qval in enumerate(Q_values):
        for i in tqdm(range(len(class_names))):
            # iterate over all instances of the class
            for inst in class_indices[i]:
                # compute preicision and recall
                precision = len([x for x in sorted_indices[inst][:Qval] if labels[x] == class_names[i]]) / Qval
                recall = len([x for x in sorted_indices[inst][:Qval] if labels[x] == class_names[i]]) / (class_frequencies[i]-1)
            

            f1_score = 0
            f1_score_2 = 0
            f1_score_3 = 0
            f1_score_05 = 0
            if precision + recall == 0:
                class_f1_scores[i][Q] += 0
            else:
                f1_score = (2 * (precision * recall) / (precision + recall))

                f1_score_05 = (1 + 0.25) * precision * recall / ( (0.25 * precision) + recall)
                f1_score_2 = (1 + 2**2) * precision * recall / ( (4 * precision) + recall)
                f1_score_3 = (1 + 3**2) * precision * recall / ( (9 * precision) + recall)

            class_f1_scores[i][Q] += f1_score / class_frequencies[i]
            total_f1_scores_05[Q] += f1_score_05 / len(features)
            total_f1_scores_1[Q] += f1_score / len(features)
            total_f1_scores_2[Q] += f1_score_2 / len(features)
            total_f1_scores_3[Q] += f1_score_3 / len(features)
    
    # plot the total f1 scores
    plt.figure(dpi=100)
    plt.title("F1 score")
    plt.plot(Q_values, total_f1_scores_05, label="F1 score beta=0.5")
    plt.plot(Q_values, total_f1_scores_1, label="F1 score beta=1")
    plt.plot(Q_values, total_f1_scores_2, label="F1 score beta=2")
    plt.plot(Q_values, total_f1_scores_3, label="F1 score beta=3")
    plt.xlabel("Q")
    plt.ylabel("F1 score")
    plt.legend()
    plt.show()

if ROC_CURVE:
    # Compute the ROC curve with query size Q, so true positive rate vs false positive rate
    Q_values = list(range(1, len(features)))

    sens_spec = []
    sens_spec_classes= [[] for _ in range(len(class_names))]

    for Q, Qval in tqdm(enumerate(Q_values)):
        specificity_total = 0
        sensitivity_total = 0
        for i in range(len(class_names)):
            specificity_class = 0
            sensitivity_class = 0

            c = class_frequencies_map[class_names[i]] - 1
            d = len(features) - 1
            s = Qval

            for inst in class_indices[i]:
                #true_positive = len([x for x in sorted_indices[inst][:Qval] if labels[x] == labels[inst]])
                true_positive = TP_matrix[inst][Qval-1]
                false_positives = s - true_positive

                true_negative = (d-c) - false_positives

                sensitivity_class += true_positive / c / (c+1)
                specificity_class += true_negative / (d-c) / (c+1)

                sensitivity_total += true_positive / c / len(features)
                specificity_total += true_negative / (d-c) / len(features)
            
            sens_spec_classes[i].append((sensitivity_class, specificity_class))
        
        sens_spec.append((sensitivity_total, specificity_total))

    # find index Q_best where sensitivity * specificity is maximal
    Q_best = np.argmax([x[0]*x[1] for x in sens_spec])

    AUC_ALL = compute_AUC(sens_spec)
    AUC_classes = []
    for i in range(len(class_names)):
        AUC_classes.append(compute_AUC(sens_spec_classes[i]))

    AUC_ind = np.argsort(AUC_classes)

    selected_classes_indices = [AUC_ind[0], AUC_ind[len(class_names)//2], AUC_ind[-1]]

    print(sens_spec_classes[selected_classes_indices[-1]])

    # plot the ROC curve
    plt.figure(dpi=100, figsize=(8,8))
    plt.plot([x[0] for x in sens_spec], [x[1] for x in sens_spec], marker='.', color="black", label=f"ALL - AUC={round(AUC_ALL, 2)}")

    for ind in selected_classes_indices:
        plt.plot([x[0] for x in sens_spec_classes[ind]], [x[1] for x in sens_spec_classes[ind]], marker='.', label=f"{class_names[ind]} - AUC={round(AUC_classes[ind],2)}")

    for i in [0, Q_best, len(sens_spec)-1]:
        plt.annotate(f'Q={Q_values[i]}', (sens_spec[i][0] + 0.01, sens_spec[i][1] + 0.01))

    # plot dashed line from (0,1) to (1, 0)
    plt.plot([0, 1], [1, 0], 'k--')

    plt.xlabel("Sensitivity")
    plt.ylabel("Specificity")
    plt.xlim(0, 1.01)
    plt.ylim(0, 1.01)
    plt.title("ROC curve")
    plt.legend()
    plt.show()
