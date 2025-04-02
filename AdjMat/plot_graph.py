import matplotlib.pyplot as plt
import numpy as np

threads = []
times = []
speedups = []

# Open the CSV file and read the contents
with open("pagerank_results_5.csv", "r") as file:
    next(file)  # Skip the header
    for line in file:
        parts = line.strip().split(",")  # Split by comma (without space)
        threads.append(int(parts[0]))  # Convert the first part to an integer
        times.append(float(parts[1]))  # Convert the second part to a float
        speedups.append(float(parts[2]))  # Convert the third part to a float

# Convert lists to numpy arrays for easier manipulation
threads = np.array(threads)
times = np.array(times)
speedups = np.array(speedups)

# Plotting Time vs Threads
plt.figure()
plt.plot(threads, times, marker='o', linestyle='-', color='b')
plt.xscale('linear')
plt.yscale('linear')
plt.xlabel('Number of Threads (Processors)')
plt.ylabel('Time (seconds)')
plt.title('Thread vs Time')
plt.grid(True)
plt.savefig('thread_vs_time.png')

# Plotting Speedup vs Threads
plt.figure()
plt.plot(threads, speedups, marker='o', linestyle='-', color='r')
plt.xscale('linear')
plt.yscale('linear')
plt.xlabel('Number of Threads (Processors)')
plt.ylabel('Speedup')
plt.title('Speedup vs Processes')
plt.grid(True)
plt.savefig('speedup_vs_processes.png')

# Uncomment this to show the plots on screen
# plt.show()
