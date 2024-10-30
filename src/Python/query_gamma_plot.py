import matplotlib.pyplot as plt
import numpy as np

# Create a list of values for gamma
gamma_values = np.linspace(0, 1, num=21)
scores = [2.46377,2.56522,2.68116,2.71014,2.81159,2.88406,3,3.14493,3.24638,3.30435,3.37681,3.36232,3.42029,3.3913,3.50725,3.47826,3.44928,3.33333,3.31884,3.10145,2.81159]


print(gamma_values)

#plot the scores
plt.figure(dpi=100)
plt.plot(gamma_values, scores)
plt.xlim(0,1)

#plot horizontal line at the maximum score
plt.axhline(max(scores), color='r', linestyle='--')

#plot the value of the max score on the graph
plt.text(-0.07, max(scores), f'{max(scores):.2f}', color='black')

# set x ticks to be the gamma values, but only include half of them
plt.xticks(np.linspace(0, 1, num=11))

plt.xlabel('Gamma')
plt.ylabel('Score')
plt.show()
