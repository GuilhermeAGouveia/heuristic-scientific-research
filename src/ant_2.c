#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#define NO_RECORDING
#include "./libs/funcoes_cec_2015/cec15_test_func.h"
#include "./libs/statistics.h"
#include "./libs/types.h"
#include "./libs/utils.h"
#include "./libs/crossover.h"
#include "./libs/log.h"

#define DEBUG(x)

#define N_ANT 200
#define N_ITER 500
#define ALPHA 2.0
#define BETA 1.0
#define RHO 0.5
#define Q 1.0
#define EPSILON 0.01

typedef struct ant
{
    double *ant_chromossome;
    double fitness;
} Ant;

double f(double *x, int n);
void initialize(Ant *ants, int n, int d);
void update_pheromones(double **pheromones, Ant *ants, int n, int d, double best);
void select_next_position(double **pheromones, double *ant_chromossome, int d);
void aco(int d);

int main()
{
    srand(time(NULL));
    int d = 10;
    aco(d);
    return 0;
}

// Objective function to be optimized
double f(double *x, int n)
{
    double y;
    cec15_test_func(x, &y, n, 1, 11);
    return y;
}

// Initialize the ants' positions randomly
void initialize(Ant *ants, int n, int d)
{
    DEBUG(printf("Initializing the ants\n");)
    for (int i = 0; i < n; i++)
    {
        ants[i].ant_chromossome = (double *)malloc(d * sizeof(double));
        for (int j = 0; j < d; j++)
        {
            ants[i].ant_chromossome[j] = random_double(-100, 100);
        }
        ants[i].fitness = f(ants[i].ant_chromossome, d);
    }
}

// Update the pheromone information in the positions visited by the ants
void update_pheromones(double **pheromones, Ant *ants, int n, int d, double best)
{
    DEBUG(printf("update_pheromones\n");)

    for (int i = 0; i < n; i++)
    {
        double delta_pheromone = Q / (ants[i].fitness - best + EPSILON);
        for (int j = 0; j < d; j++)
        {
            pheromones[i][j] = (1.0 - RHO) * pheromones[i][j] + delta_pheromone;
        }
    }
}

// Select the next ant_chromossome based on the pheromone information

void select_next_position(double **pheromones, double *ant_chromossome, int d)
{
    DEBUG(printf("select_next_position\n");)
    double total_pheromone = 0.0;
    for (int i = 0; i < N_ANT; i++)
    {
        double ant_pheromone = 0.0;
        for (int j = 0; j < d; j++)
        {
            ant_pheromone += pow(pheromones[i][j], ALPHA) * pow(1.0 / fabs(ant_chromossome[i]), BETA);
        }
        total_pheromone += ant_pheromone;
    }

    for (int i = 0; i < N_ANT; i++)
    {
        double ant_pheromone = 0.0;
        for (int j = 0; j < d; j++)
        {
            ant_pheromone += pow(pheromones[i][j], ALPHA) * pow(1.0 / fabs(ant_chromossome[i]), BETA);
        }
        double prob = ant_pheromone / total_pheromone;
        if (((double)rand() / RAND_MAX) < prob)
        {
            ant_chromossome[i] += random_double(-1, 1);
        }
    }
}

// The main ACO function
void aco(int d)
{
    DEBUG(printf("aco\n");)
    // Allocate memory for the pheromone matrix
    double **pheromones = (double **)malloc(N_ANT * sizeof(double *));
    for (int i = 0; i < N_ANT; i++)
    {
        pheromones[i] = (double *)malloc(d * sizeof(double));
        for (int j = 0; j < d; j++)
        {
            pheromones[i][j] = 1.0 / N_ANT;
        }
    }

    // Allocate memory for the ants
    Ant *ants = (Ant *)malloc(N_ANT * sizeof(Ant));

    // Initialize the ants' positions and fitness values
    initialize(ants, N_ANT, d);

    // Find the best ant and its fitness value
    int best_ant = 0;
    double best_fitness = ants[0].fitness;
    for (int i = 1; i < N_ANT; i++)
    {
        if (ants[i].fitness < best_fitness)
        {
            best_fitness = ants[i].fitness;
            best_ant = i;
        }
    }

    // Update the pheromone matrix with the best ant's path
    update_pheromones(pheromones, ants, N_ANT, d, best_fitness);

    // Iterate over the specified number of iterations
    for (int iter = 0; iter < N_ITER; iter++)
    {
        // Move each ant to a new ant_chromossome
        for (int i = 0; i < N_ANT; i++)
        {
            select_next_position(pheromones, ants[i].ant_chromossome, d);
            ants[i].fitness = f(ants[i].ant_chromossome, d);
        }

        // Update the best ant and its fitness value
        best_ant = 0;
        best_fitness = ants[0].fitness;
        for (int i = 1; i < N_ANT; i++)
        {
            if (ants[i].fitness < best_fitness)
            {
                best_fitness = ants[i].fitness;
                best_ant = i;
            }
        }

        // Update the pheromone matrix with the best ant's path
        update_pheromones(pheromones, ants, N_ANT, d, best_fitness);
    }

    // Print the best solution found
    printf("Best fitness value: %f\n", best_fitness);
    printf("Best ant_chromossome:");
    for (int i = 0; i < d; i++)
    {
        printf(" %f", ants[best_ant].ant_chromossome[i]);
    }
    printf("\n");

    // Free memory
    for (int i = 0; i < N_ANT; i++)
    {
        free(pheromones[i]);
        free(ants[i].ant_chromossome);
    }
    free(pheromones);
    free(ants);
}