import random

def generate_graph(filename, num_nodes, num_edges):
    with open(filename, "w") as f:
        f.write(f"{num_nodes} {num_edges}\n")
        edges = set()

        while len(edges) < num_edges:
            u = random.randint(0, num_nodes - 1)
            v = random.randint(0, num_nodes - 1)
            if u != v and (u, v) not in edges:  # Avoid self-loops and duplicate edges
                edges.add((u, v))
                f.write(f"{u} {v}\n")

if __name__ == "__main__":
    filename = "small_graph.txt"
    num_nodes = 5  # Adjust as needed
    num_edges = 15 # Adjust as needed
    generate_graph(filename, num_nodes, num_edges)
    print(f"Graph saved to {filename}")
