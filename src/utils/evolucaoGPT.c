#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "libs/funcoes_cec_2015/cec15_test_func.h"
#include "libs/utils.h"
#include "libs/types.h"
#include "libs/statistics.h"

#define POPULATION_SIZE 50
#define NUM_GENERATIONS 75
#define NUM_PARENTS 10
#define MUTATION_PROBABILITY 0.10
#define DIMENSION 10
#define BOUNDS_LOWER -100.0
#define BOUNDS_UPPER 100.0

void generate_population(double population[POPULATION_SIZE][DIMENSION], int pop_size, int dim, double bounds_lower, double bounds_upper)
{
    int i, j;
    for (i = 0; i < pop_size; i++)
    {
        for (j = 0; j < dim; j++)
        {
            population[i][j] = random_double(bounds_lower, bounds_upper);
        }
    }
}

double evaluate_fitness(double individual[DIMENSION], int dim)
{
    double result = 0.0;
    cec15_test_func(individual, &result, dim, 1, 3);
    return result;
}

void select_parents(double population[POPULATION_SIZE][DIMENSION], double fitness[POPULATION_SIZE], double parents[NUM_PARENTS][DIMENSION], int pop_size, int num_parents)
{
    int i, j, max_fitness_idx;
    double max_fitness;
    for (i = 0; i < num_parents; i++)
    {
        max_fitness = -1000000.0;
        for (j = 0; j < pop_size; j++)
        {
            if (fitness[j] > max_fitness)
            {
                max_fitness = fitness[j];
                max_fitness_idx = j;
            }
        }
        for (j = 0; j < DIMENSION; j++)
        {
            parents[i][j] = population[max_fitness_idx][j];
        }
        fitness[max_fitness_idx] = -1000000.0;
    }
}

void crossover(double parents[NUM_PARENTS][DIMENSION], double offspring[POPULATION_SIZE - NUM_PARENTS][DIMENSION], int num_parents, int offspring_size)
{
    int i, j, parent1_idx, parent2_idx, crossover_point = DIMENSION / 2;
    for (i = 0; i < offspring_size; i++)
    {
        parent1_idx = i % num_parents;
        parent2_idx = (i + 1) % num_parents;
        for (j = 0; j < crossover_point; j++)
        {
            offspring[i][j] = parents[parent1_idx][j];
        }
        for (j = crossover_point; j < DIMENSION; j++)
        {
            offspring[i][j] = parents[parent2_idx][j];
        }
    }
}

void mutate(double offspring[POPULATION_SIZE - NUM_PARENTS][DIMENSION], int offspring_size, double mutation_prob)
{
    int i, j;
    for (i = 0; i < offspring_size; i++)
    {
        for (j = 0; j < DIMENSION; j++)
        {
            if (random_double(0.0, 1.0) < mutation_prob)
            {
                offspring[i][j] = random_double(BOUNDS_LOWER, BOUNDS_UPPER);
            }
        }
    }
}

void generate_new_population(double population[POPULATION_SIZE][DIMENSION], double parents[NUM_PARENTS][DIMENSION], double offspring[POPULATION_SIZE - NUM_PARENTS][DIMENSION], int pop_size, int offspring_size)
{
    int i, j, population_idx = 0;
    for (i = 0; i < NUM_PARENTS; i++)
    {
        for (j = 0; j < DIMENSION; j++)
        {
            population[population_idx][j] = parents[i][j];
        }
        population_idx++;
    }
    for (i = 0; i < offspring_size; i++)
    {
        for (j = 0; j < DIMENSION; j++)
        {
            population[population_idx][j] = offspring[i][j];
        }
        population_idx++;
    }
}

double get_best_fitness(double fitness[POPULATION_SIZE])
{
    int i;
    double best_fitness = 1000000.0;
    for (i = 0; i < POPULATION_SIZE; i++)
    {
        if (fitness[i] < best_fitness)
        {
            best_fitness = fitness[i];
        }
    }
    return best_fitness;
}

individue *generate_population2(int n_populacoes, int dimension, domain domain_function)
{
    //DEBUG(printf("\ngenerate_population\n"););
    individue *populacao = malloc(n_populacoes * sizeof(individue));
    for (int i = 0; i < n_populacoes; i++)
    {
        populacao[i].chromosome = (double *)malloc(dimension * sizeof(double));
        for (int j = 0; j < dimension; j++)
        {
            populacao[i].chromosome[j] = random_double(domain_function.min, domain_function.max);
        }
        populacao[i].fitness = INFINITY;
    }

    // DEBUG(print_population(population, n_populacoes, dimension););
    return populacao;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int i, j;
    double population[POPULATION_SIZE][DIMENSION];
    double fitness[POPULATION_SIZE];
    double parents[NUM_PARENTS][DIMENSION];
    double offspring[POPULATION_SIZE - NUM_PARENTS][DIMENSION];
    generate_population(population, POPULATION_SIZE, DIMENSION, BOUNDS_LOWER, BOUNDS_UPPER);
    individue *population2 = generate_population2(POPULATION_SIZE, DIMENSION, (domain){BOUNDS_LOWER, BOUNDS_UPPER});

    for (j = 0; j < POPULATION_SIZE; j++)
    {
        //printf("Generation %d, Individual %d: \n", i, j);
        fitness[j] = evaluate_fitness(population[j], DIMENSION);
        //printf("Fitness: %f\n", fitness[j]);
    }
    for (i = 0; i < NUM_GENERATIONS; i++)
    {

        select_parents(population, fitness, parents, POPULATION_SIZE, NUM_PARENTS);
        crossover(parents, offspring, NUM_PARENTS, POPULATION_SIZE - NUM_PARENTS);
        mutate(offspring, POPULATION_SIZE - NUM_PARENTS, MUTATION_PROBABILITY);
        generate_new_population(population, parents, offspring, POPULATION_SIZE, POPULATION_SIZE - NUM_PARENTS);
        for (j = 0; j < POPULATION_SIZE; j++)
        {
            //printf("Generation %d, Individual %d: \n", i, j);
            fitness[j] = evaluate_fitness(population[j], DIMENSION);
            //printf("Fitness: %f\n", fitness[j]);
        }
        for (j = 0; j < POPULATION_SIZE; j++)
        {
            //printf("Generation %d, Individual %d: \n", i, j);
            population2[j].fitness = fitness[j];
            //printf("Fitness: %f\n", fitness[j]);
        }

        print_coords(population2, POPULATION_SIZE, i, NUM_GENERATIONS);
    }

    //printf("\n\nGeneration %d, Best Fitness: %f\n", NUM_GENERATIONS, get_best_fitness(fitness));
    return 0;
}