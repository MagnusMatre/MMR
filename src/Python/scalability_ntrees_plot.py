import matplotlib.pyplot as plt

filename = "../../res/scalability_n_trees_results/scores.txt"

n_trees = []
scores = []
build_times = []
query_times = []

# Read the scores from the file
with open(filename, "r") as f:

    for i,line in enumerate(f):
        if i == 0: continue
        content = line.split(",")
        print(content)
        n_trees.append(int(content[0]))
        scores.append(float(content[1]))
        build_times.append(float(content[2]))
        query_times.append(float(content[3][:-2]))

# Plot the build time and query time on a logarithmic x-axis with different y-scales in the same plot, build times
# are red and query times are blue

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 4))

ax1.set_xscale('log')

color = 'red'
ax1.set_xlabel('Number of trees')
ax1.set_ylabel('Build time (s)', color=color)
ax1.plot(n_trees, build_times, color=color)
ax1.tick_params(axis='y', labelcolor=color)

ax11 = ax1.twinx()
color = 'blue' # dark blue
ax11.set_ylabel('Query time (s)', color=color)
ax11.plot(n_trees, query_times, color=color)
ax11.tick_params(axis='y', labelcolor=color)

# Plot the scores on a logarithmic x-axis
ax2.set_xscale('log')
color = 'green' # dark blue
ax2.plot(n_trees, scores, color=color)
ax2.set_xlabel('Number of trees')
ax2.set_ylabel(r'Score $s$', color=color)
ax2.tick_params(axis='y', labelcolor=color)


# Set the x-ticks to be the number of trees
ax2.set_xticks(n_trees)
ax2.set_xticklabels(n_trees)

ax1.set_xticks(n_trees)
ax1.set_xticklabels(n_trees)

fig.tight_layout()
plt.show()
