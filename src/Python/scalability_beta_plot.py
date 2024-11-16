import matplotlib.pyplot as plt
import numpy as np

# Create a list of values for gamma
beta_values = np.linspace(0, 1, num=21)

scores = []
with open("../../res/scalability_beta_results/scores.txt", "r") as f:
    for line in f:
        scores.append(float(line.split()[1][:-1]))
        


print(beta_values)

#plot the scores
plt.figure(dpi=100)
plt.plot(beta_values, scores)
plt.xlim(0,1)

#plot horizontal line at the maximum score
plt.axhline(max(scores), color='r', linestyle='--')

#plot the value of the max score on the graph
plt.text(-0.07, max(scores), f'{max(scores):.2f}', color='black')

# set x ticks to be the gamma values, but only include half of them
plt.xticks(np.linspace(0, 1, num=11))

plt.xlabel(r'$\beta$')
plt.ylabel(r'Score $s$')
plt.show()