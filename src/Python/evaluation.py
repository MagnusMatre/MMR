import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from tqdm import tqdm

ANN = False

def compute_AUC(sens_spec):
    # compute the area under the curve
    auc = 0
    for i in range(1, len(sens_spec)):
        auc += (sens_spec[i][0] - sens_spec[i-1][0]) * (sens_spec[i][1] + sens_spec[i-1][1]) / 2
    return auc

def get_TP_matrix(indices, labels, K, save=True):
    # check if the TP matrix is already computed
    if ANN:
        try:
            TP_matrix = np.loadtxt("../../res/TP_matrix/matrix_ANN.txt")
            return TP_matrix
        except:
            pass
    else:
        try:
            TP_matrix = np.loadtxt("../../res/TP_matrix/matrix.txt")
            return TP_matrix
        except:
            pass

    TP_matrix = np.zeros((len(indices), len(indices[0]-1)))

    for i in tqdm(range(len(indices))):
        for q in range(K):
            if q == 0:
                if labels[indices[i][q]] == labels[i]:
                    TP_matrix[i][q] = 1
                else:
                    TP_matrix[i][q] = 0
            else:
                if labels[indices[i][q]] == labels[i]:
                    TP_matrix[i][q] = TP_matrix[i][q-1] + 1
                else:
                    TP_matrix[i][q] = TP_matrix[i][q-1]
    
    if save:
        if ANN:
            np.savetxt("../../res/TP_matrix/matrix_ANN.txt", TP_matrix)
        else:
            np.savetxt("../../res/TP_matrix/matrix.txt", TP_matrix)

    return TP_matrix

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
sorted_indices = np.delete(sorted_indices, 0, axis=1) 

if ANN:
    sorted_indices = pd.read_csv("../../res/scalability_closest_indices/closest.txt", sep=',', header=None)
    sorted_indices = np.array(sorted_indices)

#np.savetxt("../../res/TP_matrix/matrix.txt", TP_matrix)
TP_matrix = get_TP_matrix(sorted_indices, labels, 350, save=True)

print(TP_matrix)
# careful when subtracting 1 from class_frequencies[i] when you don't want to count the query instance itself

K_TIERS = False
PRECISION = False # this probably produces the "best" results in the sense that it looks the best for us
LAST_RANK = False
ACCURACY = False # useless
F1_SCORE = True 
ROC_CURVE = False # this one is interesting
MAX_RANK = False # not useful

selected_classes = ["Bicycle", "Door", "Wheel", "Bus", "BuildingNonResidential"]

if K_TIERS:
    # Compute K-tiers, so for each class compute the number of instances in each tier
    average_k_tiers = np.zeros(10)
    k_tiers = [[] for _ in range(len(class_names))] 
    for i in tqdm(range(len(class_names))):
        k_tier = np.zeros(10)
        
        # iterate over all instances of the class
        for inst in class_indices[i]:
            for k in range(10):
                frac = len([x for x in sorted_indices[inst][(class_frequencies[i]-1)*(k):(class_frequencies[i]-1)*(k+1)] if labels[x] == class_names[i]]) / (class_frequencies[i]-1)
                k_tier[k] += frac / class_frequencies[i]
                average_k_tiers[k] += frac / len(features)

        k_tiers[i] = k_tier

    k_tiers = np.array(k_tiers)

    # sort the list of k_tiers on the first tier in descending order, also keep the original indices
    sorted_indices = np.argsort(k_tiers[:,0])[::-1]

    #to_plot = [0, len(sorted_indices)//4, len(sorted_indices)//2, 3*len(sorted_indices)//4, len(sorted_indices)-1]
    to_plot = [0, len(sorted_indices)//2, len(sorted_indices)-1]
    colors = {0 : "r", 1 : "g", 2 : "b"}

    # # Plot the K-tiers
    plt.figure(figsize =(8,8))
    plt.title("K-tiers of selected classes")
    plt.plot(average_k_tiers[:5], color="black", label="Average")
    for i, ind in enumerate(to_plot):
        plt.plot(k_tiers[sorted_indices[ind]][:5], color=colors[i], label=class_names[sorted_indices[ind]])

    # change x-ticks starting from index 1
    plt.xticks(np.arange(5), np.arange(1, 6))
    plt.xlim(0,4)
    plt.ylim(0,1)
    plt.ylabel("Fraction of instances in Kth tier")
    plt.xlabel("Kth tier")
    plt.legend()
    plt.savefig("../../res/evaluation/K-tiers.png")
    plt.show()

if PRECISION:
    # compute average precision per class, i.e., the number of times the first instance of the class is from the same class

    K_vals = [1, 5, 20]

    average_precisions = np.zeros((len(class_names), len(K_vals)))
    total_average_precisions = np.zeros(len(K_vals))

    for j, K in enumerate(K_vals):
        average_precision = np.zeros(len(class_names))
        total_average_precision = 0

        for i in tqdm(range(len(class_names))):
            class_prec = 0
            # iterate over all instances of the class
            for inst in class_indices[i]:
                # find the first instance of the class in the sorted indices
                for k in range(K):
                    if sorted_indices[inst][k] in class_indices[i]:
                        class_prec += 1/K
                        total_average_precision += 1/K

            average_precision[i] = class_prec / class_frequencies[i]

        average_precisions[:, j] = average_precision
        total_average_precisions[j] = total_average_precision

        print(sum(average_precision) / len(class_names))
        print(total_average_precision / len(features))

    

    # plot the average precision in a bar chart per class
    colors = {0 : "r", 1 : "g", 2 : "b"}

    # print best value with best class
    for i in range(len(K_vals)):
        print(f"best {K_vals[i]}: ", class_names[np.argmax(average_precisions[:,i])], np.max(average_precisions[:,i]))
        print(f"worst {K_vals[i]}: ", class_names[np.argmin(average_precisions[:,i])], np.min(average_precisions[:,i]))

    plt.figure(figsize=(10,6))
    if ANN:
        plt.title(label=r"Average precision per class for ANN for different query sizes $K$")
    else:
        plt.title(label=r"Average precision per class for different query sizes $K$")
    for i,K in enumerate(K_vals):
        plt.bar(class_names, average_precisions[:, i], color=colors[i])
        plt.hlines(total_average_precisions[i] / len(features), -1, len(class_names), colors=colors[i], linestyles="dashed", label=f"Average precision K={K}: {round(total_average_precisions[i] / len(features),2):.2f}")
    plt.xlabel("Class")
    plt.xlim(-1, len(class_names))
    plt.xticks(rotation=90)
    plt.legend()
    plt.tight_layout()
    if ANN:
        plt.savefig("../../res/evaluation/average_precision_per_class_ANN.png")
    else:
        plt.savefig("../../res/evaluation/average_precision_per_class.png")
    plt.show()

if LAST_RANK:
    # Find the number of instances that are of the same rank until we hit a different class
    last_rank_vals = [1]

    last_rank = np.zeros((len(class_names), len(last_rank_vals)))
    total_last_rank = np.zeros(len(last_rank_vals))
    for k, L in enumerate(last_rank_vals):
        for i in tqdm(range(len(class_names))):
            # iterate over all instances of the class
            for inst in class_indices[i]:
                cur_last_rank = 0
                # find the first instance of the class in the sorted indices
                cur_L = 0
                for j in range(len(sorted_indices[inst])):
                    if labels[sorted_indices[inst][j]] != class_names[i]:
                        cur_L += 1

                        if cur_L >= L:
                            break

                    cur_last_rank += 1


                last_rank[i,k] += cur_last_rank / class_frequencies[i]
                total_last_rank[k] += cur_last_rank / len(features) 

    colors = {0 : "r", 1 : "g", 2 : "b"}

    # print maximal last rank found over classes
    print(np.max(last_rank, axis=0))

    # print worst class with value
    print(class_names[np.argmin(last_rank[:,0])], np.min(last_rank[:,0]))


    print(sum(last_rank) / len(class_names))
    print(total_last_rank)
    # plot the last rank in a bar chart per class
    plt.figure(figsize=(10,8))
    plt.title("Last rank")
    for i, L in enumerate(last_rank_vals):
        plt.bar(class_names, last_rank[:,i], color="blue")
        plt.hlines(total_last_rank[i], -1, len(class_names), colors="black", linestyles="dashed", label=f"Average last rank={round(total_last_rank[i],2):.2f}")
    plt.xlabel("Class")
    plt.xticks(rotation=90)
    plt.xlim(-1, len(class_names))
    plt.tight_layout()
    plt.legend()
    plt.savefig("../../res/evaluation/last_rank_per_class.png")
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
    if ANN:
        Q_values = list(range(1, 350))
    else:
        Q_values = list(range(1, 2283))

    beta = [0.5, 1, 2]

    total_f1_scores = np.zeros((len(Q_values), len(beta)))
    class_f1_scores = np.zeros((len(class_names), len(Q_values), len(beta)))

    for j, beta_val in enumerate(beta):
        for Q, Qval in enumerate(Q_values):
            for i in range(len(class_names)):
                # iterate over all instances of the class
                for inst in class_indices[i]:
                    # compute preicision and recall
                    precision = TP_matrix[inst][Qval-1] / Qval
                    recall = TP_matrix[inst][Qval-1] / (class_frequencies[i] - 1)

                    if precision + recall == 0:
                        continue
                    
                    f1_score= (1 + beta_val**2) * precision * recall / ( ( beta_val**2 * precision) + recall)

                    total_f1_scores[Q, j] += f1_score / len(features)
                    class_f1_scores[i, Q, j] += f1_score / class_frequencies[i]
    
    colors = {0 : "r", 1 : "g", 2 : "b"}

    for i, beta in enumerate(beta):
        best_Fbeta = (0, "")
        worst_Fbeta = (1, "")
        for Q, Qval in enumerate(Q_values):
            if np.max(class_f1_scores[:, Q, i]) > best_Fbeta[0]:
                best_Fbeta = (np.max(class_f1_scores[:, Q, i]), class_names[np.argmax(class_f1_scores[:, Q, i])])
            
            if np.min(class_f1_scores[:, Q, i]) < worst_Fbeta[0]:
                worst_Fbeta = (np.min(class_f1_scores[:, Q, i]), class_names[np.argmin(class_f1_scores[:, Q, i])])

        # print best and worst values with corresponding class
        print(f"best {beta}: ", best_Fbeta)
        print(f"worst {beta}: ", worst_Fbeta)

    if False:
        #plot the total f1 scores
        plt.figure(figsize=(10,6), dpi=200)
        if ANN:
            plt.title(r'$F_{\beta}$ scores for ANN')
        else:
            plt.title(r'$F_{\beta}$ scores')
        for j, beta_val in enumerate(beta):
            plt.plot(Q_values, total_f1_scores[:, j], color=colors[j], label=r"$\beta=$"+f"{beta_val}")

        # find the index Q_best where the F_beta scores are maximal and annotate them with the corresponding value
        for i in range(len(beta)):
            Q_best = np.argmax(total_f1_scores[:, i])
            print(Q_best, total_f1_scores[Q_best, i])
            plt.annotate(r'$K$=' + f"{Q_best}", (Q_best, max(total_f1_scores[:, i]) *1.01))

        plt.xlabel(r"Query size $K$")
        plt.ylabel(r'$F_{\beta}$ score')
        plt.xlim(0, 200)
        plt.legend()
        if ANN:
            plt.savefig("../../res/evaluation/F_beta_scores_ANN.png")
        else:
            plt.savefig("../../res/evaluation/F_beta_scores.png")
        plt.show()

    if False:
        print(class_f1_scores[1,:,0])

        # for each beta, find the optimal values for Q for each class with corresponding value
        Q_optimal_vals = np.zeros((len(class_names), len(beta)))
        for j, beta_val in enumerate(beta):
            for i in range(len(class_names)):
                Q_optimal = np.argmax(class_f1_scores[i,:,j])
                Q_optimal_vals[i, j] = Q_optimal

        # plot the optimal values for Q for each class
        plt.figure(figsize=(10,6))
        plt.title(r"Values of query size $K$ that maximize the $F_1$ scores")
        # bar_width = 0.3
        # bar_positions = np.arange(len(class_names))

        # for j, beta_val in enumerate(beta):
        #     plt.bar(bar_positions + j * bar_width, Q_optimal_vals[:, j], width=bar_width, color=colors[j], label=r'$\beta$='+str(beta_val))
        plt.bar(class_names, Q_optimal_vals[:, 1], color="blue", label=r"$K_{opt}$")
        
        # with transparant red bar, also plot the class sizes
        plt.bar(class_names, class_frequencies, color="red", alpha=0.5, label="Class size")
        #plt.xticks(bar_positions + bar_width, class_names)

        plt.xlabel("Class")
        plt.xticks(rotation=90)
        plt.xlim(-1, len(class_names))
        plt.ylim(0, 1.05 * max(Q_optimal_vals[:, 1]))
        plt.ylabel(r"Optimal query size $K_{opt}$")
        #plt.legend()
        plt.tight_layout()
        plt.legend()
        plt.savefig("../../res/evaluation/optimal_query_size_F1_score.png")
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

    selected_classes_indices = [AUC_ind[0], AUC_ind[len(class_names)//4], AUC_ind[len(class_names)//2], AUC_ind[3*len(class_names)//4], AUC_ind[-1]]

    colors = {0 : "r", 1 : "g", 2 : "b", 3 : "y", 4 : "m"}

    # plot the ROC curve
    plt.figure(figsize=(8,8))
    plt.title("ROC curve")
    plt.plot([x[0] for x in sens_spec], [x[1] for x in sens_spec], marker='.', color="black", label=f"ALL - AUC={round(AUC_ALL, 2):.2f}")
    for i,ind in enumerate(selected_classes_indices):
        plt.plot([x[0] for x in sens_spec_classes[ind]], [x[1] for x in sens_spec_classes[ind]],
                  marker='.',
                  color=colors[i],
                  label=f"{class_names[ind]} - AUC={round(AUC_classes[ind],2):.2f}",
                  alpha=0.8)
    for i in [0, Q_best, len(sens_spec)-1]:
        plt.annotate(r'$K=$'+f"{Q_values[i]}", (sens_spec[i][0] + 0.01, sens_spec[i][1] + 0.02))

    # plot dashed line from (0,1) to (1, 0)
    plt.plot([0, 1], [1, 0], 'k--')

    plt.xlabel("Sensitivity")
    plt.ylabel("Specificity")
    plt.xlim(0, 1.02)
    plt.ylim(0, 1.02)
    plt.legend()
    plt.savefig("../../res/evaluation/ROC_curve.png")
    plt.show()

if MAX_RANK:
    # compute the index at which the last instance of the class is found and divide by the number of instances of that class
    max_rank = np.zeros(len(class_names))
    total_max_rank = 0

    for i in tqdm(range(len(class_names))):
        for inst in class_indices[i]:
            for j in range(len(features)):
                if TP_matrix[inst][j] == class_frequencies[i] - 1:
                    max_rank[i] += j / (class_frequencies[i]**2)
                    total_max_rank += j / class_frequencies[i] / len(features)
                    break
    
    print(max_rank)
    print(total_max_rank)

    # plot the max rank in a bar chart per class
    plt.figure(figsize=(10,6))
    plt.title("Max rank")
    plt.bar(class_names, max_rank, color="blue")
    plt.hlines(total_max_rank, -1, len(class_names), linestyles="dashed", color="black", label=f"Average max rank: {round(total_max_rank,2):.2f}")
    plt.xlabel("Class")
    plt.xticks(rotation=90)
    plt.xlim(-1, len(class_names))
    plt.tight_layout()
    plt.legend()
    plt.savefig("../../res/evaluation/max_rank_per_class.png")
    plt.show()