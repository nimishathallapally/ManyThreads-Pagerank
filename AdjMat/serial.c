#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define DAMPING_FACTOR 0.85
#define THRESHOLD 0.0001

typedef struct
{
    int **graph;
    int n;
    int *outLinks;
    int **inLinks;
} Graph;

// Function to allocate memory for the graph
Graph *createGraph(int n)
{
    Graph *g = (Graph *)malloc(sizeof(Graph));
    g->n = n;

    g->graph = (int **)malloc(n * sizeof(int *));
    g->outLinks = (int *)calloc(n, sizeof(int));
    g->inLinks = (int **)malloc(n * sizeof(int *));

    for (int i = 0; i < n; i++)
    {
        g->graph[i] = (int *)calloc(n, sizeof(int));
        g->inLinks[i] = (int *)calloc(n, sizeof(int));
    }

    return g;
}

// Function to free allocated memory for the graph
void freeGraph(Graph *g)
{
    for (int i = 0; i < g->n; i++)
    {
        free(g->graph[i]);
        free(g->inLinks[i]);
    }
    free(g->graph);
    free(g->inLinks);
    free(g->outLinks);
    free(g);
}

// Function to read the graph from a file
Graph *readGraphFromFile(const char *filename)
{
    int n, edges, u, v;
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error: Could not open file %s\n", filename);
        return NULL;
    }

    if (fscanf(file, "%d %d", &n, &edges) != 2)
    {
        printf("Error: Invalid file format.\n");
        fclose(file);
        return NULL;
    }

    Graph *g = createGraph(n);

    for (int i = 0; i < edges; i++)
    {
        if (fscanf(file, "%d %d", &u, &v) != 2 || u >= n || v >= n)
        {
            printf("Error: Invalid edge or out-of-bounds node.\n");
            fclose(file);
            freeGraph(g);
            return NULL;
        }
        g->graph[u][v] = 1;
        g->outLinks[u]++;
        g->inLinks[v][u] = 1;
    }

    fclose(file);
    return g;
}

// Function to initialize PageRank values
void initializePageRank(Graph *g, double *opg)
{
    for (int i = 0; i < g->n; i++)
    {
        opg[i] = 1.0 / g->n;
    }
}

// Function to compute contribution from dangling nodes
double computeDanglingContribution(Graph *g, double *opg)
{
    double dp = 0.0;
    for (int p = 0; p < g->n; p++)
    {
        if (g->outLinks[p] == 0)
        {
            dp += (DAMPING_FACTOR * opg[p]) / g->n;
        }
    }
    return dp;
}

// Function to update PageRank values
void updatePageRank(Graph *g, double *opg, double *npg, double dp)
{
    for (int p = 0; p < g->n; p++)
    {
        npg[p] = dp + (1.0 - DAMPING_FACTOR) / g->n;

        for (int ip = 0; ip < g->n; ip++)
        {
            if (g->inLinks[p][ip])
            {
                npg[p] += (DAMPING_FACTOR * opg[ip]) / g->outLinks[ip];
            }
        }
    }
}

// Function to check convergence
int hasConverged(double *opg, double *npg, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (fabs(npg[i] - opg[i]) > THRESHOLD)
        {
            return 0; // Not yet converged
        }
    }
    return 1; // Converged
}

// Function to compute PageRank
void computePageRank(Graph *g, int maxIterations)
{
    double *opg = (double *)malloc(g->n * sizeof(double));
    double *npg = (double *)malloc(g->n * sizeof(double));

    initializePageRank(g, opg);

    while (maxIterations > 0)
    {
        double dp = computeDanglingContribution(g, opg);
        updatePageRank(g, opg, npg, dp);

        if (hasConverged(opg, npg, g->n))
        {
            break;
        }

        for (int i = 0; i < g->n; i++)
        {
            opg[i] = npg[i];
        }

        maxIterations--;
    }

    // printf("PageRank values:\n");
    // for (int i = 0; i < g->n; i++)
    // {
    //     printf("Node %d: %.6f\n", i, opg[i]);
    // }

    free(opg);
    free(npg);
}

int main()
{
    char filename[100];
    int iterations;

    printf("Enter the filename: ");
    scanf("%s", filename);

    Graph *g = readGraphFromFile(filename);
    if (!g)
    {
        return 1;
    }

    printf("Enter the number of iterations: ");
    scanf("%d", &iterations);

    clock_t start_time = clock();
    computePageRank(g, iterations);
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("PageRank computation time: %.6f seconds\n", elapsed_time);

    freeGraph(g);

    return 0;
}
