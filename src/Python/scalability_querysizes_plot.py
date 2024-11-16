import matplotlib.pyplot as plt

filename_ann = "../../res/scalability_querysize_results/scores.txt"
filename_exhaustive = "../../res/scalability_normalquery/querysize_timing.txt"

K_values = []
scores_ann = []
query_times_ann = []
scores_exh = []
query_times_exh = []

# Read the scores from the file
with open(filename_ann, "r") as f:

    for i,line in enumerate(f):
        if i == 0: continue
        content = line.split(",")
        print(content)
        K_values.append(int(content[0]))
        scores_ann.append(float(content[1]))
        query_times_ann.append(float(content[2][:-2]))

with open(filename_exhaustive, "r") as f:

    for i,line in enumerate(f):
        if i == 0: continue
        content = line.split(",")
        scores_exh.append(float(content[1]))
        query_times_exh.append(float(content[2][:-2]))

# Plot the build time and query time on a logarithmic x-axis with different y-scales in the same plot, build times
# are red and query times are blue

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 4))

ax1.set_xscale('log')

color_ann = 'red'
color_exh = 'blue'

ax1.set_ylabel('Query time (s)')
ax1.set_xlabel(r'Query size $K$')
ax1.set_yscale('log')
ax1.plot(K_values, query_times_ann, color=color_ann, label="ANN")
ax1.plot(K_values, query_times_exh, color=color_exh, label="Exhaustive")
ax1.tick_params(axis='y')

# Plot the scores on a logarithmic x-axis
ax2.set_xscale('log')
ax2.plot(K_values, scores_ann, color=color_ann, label="ANN")
ax2.plot(K_values, scores_exh, color=color_exh, label="Exhaustive")
ax2.set_xlabel(r'Query size $K$')
ax2.set_ylabel(r'Normalized score $s$')
ax2.tick_params(axis='y')


# Set the x-ticks to be the number of trees
ax2.set_xticks([1, 5, 10, 50, 100, 350])
ax2.set_xticklabels([1, 5, 10, 50, 100, 350])

ax1.set_xticks([1, 5, 10, 50, 100, 350])
ax1.set_xticklabels([1, 5, 10, 50, 100, 350])

ax1.set_yticks([5, 10, 50, 100, 250, 1000])
ax1.set_yticklabels([5, 10, 50, 100, 250, 1000])

fig.tight_layout()
plt.legend()
plt.show()
