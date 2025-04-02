import random

def generate_graph(filename, num_nodes, num_edges, p):
    with open(filename, "w") as f:
        f.write(f"{num_nodes} {num_edges}\n")
        edges = set()

        while len(edges) < num_edges:
            u = random.randint(0, num_nodes - 1)
            v = random.randint(0, num_nodes - 1)

            # Ensure u != v and the edge is not already in the set
            if u != v and (u, v) not in edges and (v, u) not in edges:
                if random.random() < p:
                    edges.add((u, v))
                    f.write(f"{u} {v}\n")

        # If edges are still less than num_edges after the loop, keep adding edges
        # to fill the desired number of edges
        while len(edges) < num_edges:
            u = random.randint(0, num_nodes - 1)
            v = random.randint(0, num_nodes - 1)
            if u != v and (u, v) not in edges and (v, u) not in edges:
                edges.add((u, v))
                f.write(f"{u} {v}\n")

if __name__ == "__main__":
    filename = "graph2.txt"
    num_nodes = 5000  
    num_edges = 50000
    probability = 0.5  

    generate_graph(filename, num_nodes, num_edges, probability)
    print(f"Graph saved to {filename}")
