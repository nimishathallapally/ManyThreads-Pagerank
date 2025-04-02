import pandas as pd
import matplotlib.pyplot as plt

# Define the number of vertices and corresponding filenames
vertex_counts = [1000, 5000, 10000, 20000, 50000]
filenames = [f"pagerank_results_{i}.csv" for i in range(1, len(vertex_counts) + 1)]

# Initialize a figure for plotting
plt.figure(figsize=(10, 6))

# Loop through each file corresponding to a vertex count
for i, filename in enumerate(filenames):
    # Read CSV data using pandas
    data = pd.read_csv(filename)
    
    # Extract the thread counts and speedups from the CSV
    threads = data['Threads']
    speedups = data['Speedup']
    
    # Plot the speedup for this vertex count
    plt.plot(threads, speedups, marker='o', label=f'{vertex_counts[i]} vertices')

# Customize the plot
plt.xlabel('Number of Threads')
plt.ylabel('Speedup')
plt.title('Speedup vs Threads for Different Vertex Counts')
plt.legend()
plt.grid(True)
plt.xscale('linear')
plt.yscale('linear')

# Save the plot as a PNG file
plt.savefig('figure.png')

# Show the plot
# plt.show()
