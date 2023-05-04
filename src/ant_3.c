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
#define N_ITER 1000
#define ALPHA 2.0
#define BETA 1.0
#define RHO 0.5
#define Q 1.0
#define EPSILON 0.01

double *sigma;

typedef struct ant
{
    double *ant_chromossome;
    double fitness;
} Ant;
double abs_double(double x);
double f(double *x, int n);
void initialize(Ant *ants, int n, int d);
void update_pheromones(double **pheromones, Ant *ants, int n, int d, int best);
void select_next_position(double **pheromones, Ant *ants, int d);
void aco(int d);

int main()
{
    srand(10);
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
void update_pheromones(double **pheromones, Ant *ants, int n, int d, int best)
{
    DEBUG(printf("update_pheromones\n");)

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < d; j++)
        {
            pheromones[i][j] = (1 / sqrt(abs_double(2 * sigma[j] * PI))) * exp(pow((pheromones[best][j] - pheromones[i][j]), 2) / (-2 * pow(sigma[j],2)));
            if(pheromones[i][j] == 0)
              pheromones[i][j] = random_double(0.01,0.09);
            //printf("pheromoneo: %lf\n",pheromones[i][j] );
        }
        //printf("\n\n");

    }
}

// Select the next ant_chromossome based on the pheromone information


double sum_pheromone_dimension(double **pheromones, int d)
{
    double sum = 0.0;
    for (int i = 0; i < N_ANT; i++)
    {
        sum += pheromones[i][d];
    }
    return sum;
}

void select_next_position(double **pheromones, Ant *ants, int d)
{
    DEBUG(printf("select_next_position\n");)
    double sum_pheromone[d];

    for (int i = 0; i < d; i++)
    {
        sum_pheromone[i] = sum_pheromone_dimension(pheromones, i);
    }
    //printf("Select_Sum_Pheromone:%lf\n", sum_pheromone[1]);

    for (int i = 0; i < N_ANT; i++)
    {
        for (int j = 0; j < d; j++)
        {
            ants[i].ant_chromossome[j] = pheromones[i][j] / sum_pheromone[j];
        }
        ants[i].fitness = f(ants[i].ant_chromossome, d);
    }
}

double abs_double(double x) {
    if (x < 0.0) {
        return -x;
    } else {
        return x;
    }
}

double sigma_sums(Ant *ants, int dimension, int best_ant){
    long double sum_one = 0.0, sum_two = 0.0;
    long double subtraction_fitness = 0.0;

    for(int i = 0; i < N_ANT; i++){
        subtraction_fitness = ants[best_ant].fitness - ants[i].fitness;
        if(subtraction_fitness == 0)
           subtraction_fitness = 0.001;
        sum_one += pow(ants[best_ant].ant_chromossome[dimension] - ants[i].ant_chromossome[dimension],2)/subtraction_fitness;
        sum_two += 1/subtraction_fitness;
        //printf("sum_one-:%Lf\n", sum_one);
       // printf("sum_two-:%Lf\n", sum_two);
        //printf("subtract-:%Lf\n", subtraction_fitness);
    }
   // printf("sum_one-FINAL:%Lf\n", sum_one);
   // printf("sum_two-FINAL:%Lf\n", sum_two);
    sum_one = sqrt(abs_double(sum_one/sum_two));
   // printf("sigma-:%Lf\n", sum_one);
   return sum_one;
}

void update_sigma(Ant *ants, int d, int best_ant){
    for(int i = 0; i < d; i++){
        sigma[i] = sigma_sums(ants, i, best_ant);

    }
}

void print_sigma(){
    for(int i = 0; i < 10; i++){
        printf("Sigma %d: %lf\n", i, sigma[i]);
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
    sigma = (double *)malloc(d * sizeof(double));
    update_sigma(ants, d, best_ant);
   // printf("Sigma:%lf\n", sigma[0]);

    // Update the pheromone matrix with the best ant's path
    update_pheromones(pheromones, ants, N_ANT, d, best_ant);
    //print_sigma();

    // Iterate over the specified number of iterations
    for (int iter = 0; iter < N_ITER; iter++)
    {
        if(iter < 10)
        {
            printf("Fitness_best:%lf\n", best_fitness);
        }
        // Move each ant to a new ant_chromossome

        select_next_position(pheromones, ants, d);

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
        update_pheromones(pheromones, ants, N_ANT, d, best_ant);
        update_sigma(ants, d, best_ant);
    }

    // Print the best solution found
    printf("Best fitness value: %lf\n", best_fitness);
    printf("Best ant_chromossome:");
    for (int i = 0; i < d; i++)
    {
        printf(" %lf", ants[best_ant].ant_chromossome[i]);
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