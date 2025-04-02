#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define DAMPING_FACTOR 0.85
#define THRESHOLD 0.0001

typedef struct
{
    int **graph;
    int n;
    int *outLinks;
    int **inLinks;
} Graph;

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

void initializePageRank(Graph *g, double *opg)
{
    int i;
    // Parallelize initialization since each node's PageRank value is independent.
    #pragma omp parallel for shared(opg,g) private(i)
    for (i = 0; i < g->n; i++)
    {
        opg[i] = 1.0 / g->n;
    }
}

double computeDanglingContribution(Graph *g, double *opg)
{
    double dp = 0.0;
    int p;
    //  Parallelize sum computation across nodes with reduction to avoid race conditions.
    #pragma omp parallel for shared(g, opg) reduction(+ : dp) private(p)
    for (p = 0; p < g->n; p++)
    {
        if (g->outLinks[p] == 0)
        {
            dp += (DAMPING_FACTOR * opg[p]) / g->n;
        }
    }
    return dp;
}

void updatePageRank(Graph *g, double *opg, double *npg, double dp)
{
    // Parallelizing this ensures each node computes its new rank independently.
    int ip,p;
    #pragma omp parallel for shared(g, opg, npg, dp) private(p, ip)
    for ( p = 0; p < g->n; p++)
    {
        npg[p] = dp + (1.0 - DAMPING_FACTOR) / g->n;
        for (ip = 0; ip < g->n; ip++)
        {
            if (g->inLinks[p][ip])
            {
                npg[p] += (DAMPING_FACTOR * opg[ip]) / g->outLinks[ip];
            }
        }
    }
}

int hasConverged(double *opg, double *npg, int n)
{
    int converged = 1;
    int i;
    // Parallel check for convergence
    #pragma omp parallel for shared(opg, npg) reduction(&& : converged) private(i)
    for (i = 0; i < n; i++)
    {
        if (fabs(npg[i] - opg[i]) > THRESHOLD)
        {
            converged = 0;
        }
    }
    return converged;
}

void computePageRank(Graph *g, int maxIterations, int threads)
{
    omp_set_num_threads(threads);
    double *opg = (double *)malloc(g->n * sizeof(double));
    double *npg = (double *)malloc(g->n * sizeof(double));

    initializePageRank(g, opg);

    while (maxIterations > 0)
    {
        double dp = computeDanglingContribution(g, opg);
        int i;
        updatePageRank(g, opg, npg, dp);

        if (hasConverged(opg, npg, g->n))
        {
            break;
        }

        // Parallel copying of npg to opg for the next iteration
        #pragma omp parallel for shared(opg, npg) private(i)
        for (i = 0; i < g->n; i++)
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
    int thread_counts[] = {1, 2, 4, 6, 8, 10, 12, 16, 20, 32, 64};

    printf("Enter the filename: ");
    scanf("%s", filename);

    Graph *g = readGraphFromFile(filename);
    if (!g)
    {
        return 1;
    }

    printf("Enter the number of iterations: ");
    scanf("%d", &iterations);

    FILE *fout = fopen("pagerank_results_2.csv", "w");
    fprintf(fout, "Threads,Time,Speedup,Parallel Fraction\n");

    double first_time = 0.0;
    for (int j = 0; j < sizeof(thread_counts) / sizeof(thread_counts[0]); j++)
    {
        int threads = thread_counts[j];
        double start_time = omp_get_wtime();

        computePageRank(g, iterations, threads);

        double end_time = omp_get_wtime();
        double time_taken = end_time - start_time;
        double speedup = (threads == 1) ? 1.0 : first_time / time_taken;
        double parallel_fraction = (1 - (1 / speedup)) / (1 - (1.0 / threads));

        if (threads == 1)
            first_time = time_taken;

        fprintf(fout, "%d,%f,%f,%f\n", threads, time_taken, speedup, parallel_fraction);
        printf("Threads = %d, Time taken = %f, Speedup = %f, Parallel Fraction = %f\n", threads, time_taken, speedup, parallel_fraction);
    }

    fclose(fout);
    freeGraph(g);
    return 0;
}
