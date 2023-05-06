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
#define TAX_EVAPORATE 0.1
#define NUM_CANDIDATES 10
#define P_EXPLOITATION 0.3

double *sigma;
double **pheromones_candidates;
double **pheromones_best;

typedef struct ant
{
    double *ant_chromossome;
    double fitness;
} Ant;
Ant *candidates;

double abs_double(double x);
double f(double *x, int n);
void initialize(Ant *ants, int n, int d);
void update_pheromones(double **pheromones, Ant *ants, int n, int d);
void select_next_position(double **pheromones, Ant *ants, int d);
void aco(int d);

int main()
{
    srand(time(NULL));
    int d = 10;
    aco(d);
    return 0;
}

void print_ant(Ant *ant, int dimension, int id)
{

    for (int j = 0; j < dimension; j++)
    {
        printf("%lf ", ant[id].ant_chromossome[j]);
    }
    printf("-> %lf\n", ant[id].fitness);
}

void print_ants(Ant *ant, int dimension)
{
    for (int i = 0; i < N_ANT; i++)
    {
        print_ant(ant, dimension, i);
    }
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
void evaporate_pheromones(double **pheromones, int dimension)
{
    for (int i = 0; i < N_ANT; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            pheromones[i][j] = pheromones[i][j] * (1 - TAX_EVAPORATE);
        }
    }
}

// Update the pheromone information in the positions visited by the ants
void update_pheromones(double **pheromones, Ant *ants, int n, int d)
{
    DEBUG(printf("update_pheromones\n");)

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < d; j++)
        {
            pheromones[i][j] = (1 / sqrt(2 * sigma[j] * PI)) * exp(pow((pheromones_best[0][j] - pheromones[i][j]), 2) / (-2 * pow(sigma[j], 2)));
            // printf("pheromoneo: %lf\n",pheromones[i][j] );
        }
        // printf("\n\n");
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

double sum_pheromone_ant(double **pheromones, int d, int id)
{
    double sum = 0.0;
    for (int i = 0; i < d; i++)
    {
        sum += pheromones[id][i];
    }
    return sum;
}

double sum_pheromone_ants(double **pheromones, int d, int n_ants)
{
    double sum = 0.0;
    for (int j = 0; j < n_ants; j++)
    {
        for (int i = 0; i < d; i++)
        {
            sum += pheromones[j][i];
        }
    }
    return sum;
}

void select_next_position2(double **pheromones, Ant *ants, int d)
{
    DEBUG(printf("select_next_position\n");)
    double sum_pheromone[d], prob;

    for (int i = 0; i < d; i++)
    {
        sum_pheromone[i] = sum_pheromone_dimension(pheromones, i);
    }
    // printf("Select_Sum_Pheromone:%lf\n", sum_pheromone[1]);

    for (int i = 0; i < N_ANT; i++)
    {
        for (int j = 0; j < d; j++)
        {
            prob = pheromones[i][j] / sum_pheromone[j];

            if (((double)rand() / RAND_MAX) < prob)
            {
                ants[i].ant_chromossome[j] += random_double(-5, 5);
            }
        }
        ants[i].fitness = f(ants[i].ant_chromossome, d);
    }
}

void saved_chosen(Ant *ants, int id_origin, int id_new, int d)
{
    for (int i = 0; i < d; i++)
    {
        ants[id_origin].ant_chromossome[i] = candidates[id_new].ant_chromossome[i];
    }
}

void select_next_position(double **pheromones, Ant *ants, int d)
{
    DEBUG(printf("select_next_position\n");)
    double sum_pheromone[d], prob, mean = 0.0;
    double distance, current_pheromone, delta;
    double chosen_porcent, probability_porcent = 0.0;

    for (int i = 0; i < d; i++)
    {
        sum_pheromone[i] = sum_pheromone_dimension(pheromones, i);
    }
    // Para cada formiga gera N candidatos com base no feromonio
    for (int i = 0; i < N_ANT; i++)
    {
        for (int k = 0; k < NUM_CANDIDATES; k++)
        {
            for (int j = 0; j < d; j++)
            {

                current_pheromone = ants[i].ant_chromossome[j];
                mean = (sum_pheromone[j] - current_pheromone) / N_ANT - 1;
                distance = abs_double(mean - current_pheromone);
                delta = distance / current_pheromone;
                // Cij = Xi,j + Δij * (m - Xi,j).
                candidates[k].ant_chromossome[j] = current_pheromone + delta * (mean - current_pheromone);
                if (random_double(0, 1) <= P_EXPLOITATION)
                    candidates[k].ant_chromossome[j] = random_double(-100, 100);
            }
        }
        update_pheromones(pheromones_candidates, candidates, NUM_CANDIDATES, d);
        chosen_porcent = random_double(0, 1);
        // Calcula a probabilidade de cada novo ponto de ser escolhido como novo local para a formiga
        for (int y = 0; y < NUM_CANDIDATES; y++)
        {
            probability_porcent += sum_pheromone_ant(pheromones_candidates, d, y) / sum_pheromone_ants(pheromones_candidates, d, NUM_CANDIDATES);
            if (probability_porcent <= chosen_porcent)
            {
                saved_chosen(ants, i, y, d);
                ants[i].fitness = f(ants[i].ant_chromossome, d);
                break;
            }
        }
        probability_porcent = 0.0;
    }
}

double abs_double(double x)
{
    if (x < 0.0)
    {
        return -x;
    }
    else
    {
        return x;
    }
}

double sigma_sums(Ant *ants, int dimension, Ant *best_ant)
{
    long double sum_one = 0.0, sum_two = 0.0;
    long double subtraction_fitness = 0.0;

    for (int i = 0; i < N_ANT; i++)
    {
        subtraction_fitness = best_ant->fitness - ants[i].fitness;
        if (subtraction_fitness == 0)
            subtraction_fitness = 0.001;
        sum_one += pow(best_ant->ant_chromossome[dimension] - ants[i].ant_chromossome[dimension], 2) / subtraction_fitness;
        sum_two += 1 / subtraction_fitness;
        // printf("sum_one-:%Lf\n", sum_one);
        // printf("sum_two-:%Lf\n", sum_two);
        // printf("subtract-:%Lf\n", subtraction_fitness);
    }
    // printf("sum_one-FINAL:%Lf\n", sum_one);
    // printf("sum_two-FINAL:%Lf\n", sum_two);
    sum_one = sqrt(abs_double(sum_one / sum_two));
    // printf("sigma-:%Lf\n", sum_one);
    return sum_one;
}

void update_sigma(Ant *ants, int d, Ant *best_ant)
{
    for (int i = 0; i < d; i++)
    {
        sigma[i] = sigma_sums(ants, i, best_ant);
    }
}

void print_sigma()
{
    for (int i = 0; i < 10; i++)
    {
        printf("Sigma %d: %lf\n", i, sigma[i]);
    }
}

void best_ant_check(Ant *ants, Ant *best_ant, double **pheromones, int d)
{
    int confirm = 0;
    for (int i = 0; i < N_ANT; i++)
    {
        if (ants[i].fitness <= best_ant->fitness)
            confirm = 1;
    }
    if (!confirm)
    {
        for (int i = 0; i < d; i++)
        {
            ants[0].ant_chromossome[i] = best_ant->ant_chromossome[i];
            pheromones[0][i] = pheromones_best[0][i];
        }
        ants[0].fitness = best_ant->fitness;
    }
}

void catch_best_ant(Ant *ants, Ant *best_antt, double **pheromones, int d)
{
    double best_fitness = best_antt->fitness;
    int best_ant = -1;

    for (int i = 1; i < N_ANT; i++)
    {
        if (ants[i].fitness < best_fitness)
        {
            best_fitness = ants[i].fitness;
            best_ant = i;
        }
    }

    if (best_ant != -1)
    {

        for (int i = 0; i < d; i++)
        {
            best_antt->ant_chromossome[i] = ants[best_ant].ant_chromossome[i];
            pheromones_best[0][i] = pheromones[best_ant][i];
        }
        best_antt->fitness = ants[best_ant].fitness;
    }
}

// The main ACO function
void aco(int d)
{
    DEBUG(printf("aco\n");)
    // Allocate memory for the pheromone matrix
    double **pheromones = (double **)malloc(N_ANT * sizeof(double *));
    pheromones_candidates = (double **)malloc(NUM_CANDIDATES * sizeof(double *));
    pheromones_best = (double **)malloc(1 * sizeof(double *));
    for (int i = 0; i < N_ANT; i++)
    {
        pheromones[i] = (double *)malloc(d * sizeof(double));
        for (int j = 0; j < d; j++)
        {
            pheromones[i][j] = 1.0 / N_ANT;
        }
    }

    for (int i = 0; i < NUM_CANDIDATES; i++)
    {
        pheromones_candidates[i] = (double *)malloc(d * sizeof(double));
        for (int j = 0; j < d; j++)
        {
            pheromones_candidates[i][j] = 1.0 / NUM_CANDIDATES;
        }
    }

    pheromones_best[0] = (double *)malloc(d * sizeof(double));
    for (int j = 0; j < d; j++)
    {
        pheromones_best[0][j] = 1.0 / NUM_CANDIDATES;
    }

    // Allocate memory for the ants
    Ant *ants = (Ant *)malloc(N_ANT * sizeof(Ant));
    Ant *best_ant = (Ant *)malloc(1 * sizeof(Ant));
    candidates = (Ant *)malloc(NUM_CANDIDATES * sizeof(Ant));

    // Initialize the ants' positions and fitness values
    initialize(ants, N_ANT, d);
    initialize(best_ant, 1, d);
    initialize(candidates, NUM_CANDIDATES, d);

    // Find the best ant and its fitness value
    catch_best_ant(ants, best_ant, pheromones, d);

    sigma = (double *)malloc(d * sizeof(double));
    update_sigma(ants, d, best_ant);

    // Update the pheromone matrix with the best ant's path
    update_pheromones(pheromones, ants, N_ANT, d);

    // Iterate over the specified number of iterations

    for (int iter = 0; iter < N_ITER; iter++)
    {

        DEBUG(print_ant(ants, d, best_ant););
        printf("Best_fitness: %lf\n", best_ant->fitness);
        // Move each ant to a new ant_chromossome
        select_next_position(pheromones, ants, d);
        evaporate_pheromones(pheromones, d);

        // Update the best ant and its fitness value
        catch_best_ant(ants, best_ant, pheromones, d);
        //Verifica se a melhor posição não foi perdida
        best_ant_check(ants,best_ant,pheromones, d);
        // Update the pheromone matrix with the best ant's path
        update_pheromones(pheromones, ants, N_ANT, d);
        update_sigma(ants, d, best_ant);
    }

    // Print the best solution found
    // print_ants(ants, d);
    printf("Best fitness value: %lf\n", best_ant->fitness);
    printf("Best ant_chromossome:");
    for (int i = 0; i < d; i++)
    {
        printf(" %lf", best_ant->ant_chromossome[i]);
    }
    printf("\n");

    // Free memory
    for (int i = 0; i < N_ANT; i++)
    {
        free(pheromones[i]);
        free(ants[i].ant_chromossome);
    }

    for (int i = 0; i < NUM_CANDIDATES; i++)
    {
        free(pheromones_candidates[i]);
        free(candidates[i].ant_chromossome);
    }
    free(pheromones);
    free(ants);
    free(pheromones_candidates);
    free(candidates);
    free(sigma);
    free(best_ant);
    free(pheromones_best[0]);
}