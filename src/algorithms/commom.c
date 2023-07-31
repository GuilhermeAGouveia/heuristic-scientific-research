#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#define NO_RECORDING
#include "../libs/funcoes_cec_2015/cec15_test_func.h"
#include "../libs/statistics.h"
#include "../libs/types.h"
#include "../libs/utils.h"
#include "../libs/crossover.h"

#include "../libs/log.h"
#define STATISTICS(x)
#define DEBUG(x)
#define LOG(x)


void fitness(individuo *individuo, int dimension, int function_number)
{
    // individuo.fitness = real_function(individuo.chromosome, dimension);
    cec15_test_func(individuo->chromosome, &individuo->fitness, dimension, 1, function_number);
    // double x = individuo->chromosome[0];
    // double y = individuo->chromosome[1];
    // individuo->fitness = pow(x, 2) + pow(y, 2) - cos(18 * x) - cos(18 * y);
    // printf("fitness: %f\n", individuo->fitness);
}

int comparador_individuo(const void *a, const void *b)
{
    individuo *v1 = (individuo *)a;
    individuo *v2 = (individuo *)b;

    return v1->fitness < v2->fitness;
}


individuo *generate_population(int n_individuos, int dimension, domain domain_function, int function_number)
{
    DEBUG(printf("\ngenerate_population\n"););
    individuo *population = malloc(n_individuos * sizeof(individuo));
    for (int i = 0; i < n_individuos; i++)
    {
        population[i].chromosome = (double *)malloc(dimension * sizeof(double));
        population[i].velocidade = (double *)malloc(dimension * sizeof(double));
        for (int j = 0; j < dimension; j++)
        {
            population[i].chromosome[j] = random_double(domain_function.min, domain_function.max);
            population[i].velocidade[j] = 0;
        }
        fitness(&population[i], dimension, function_number);
    }
    return population;
}

populacao *generate_island(int island_size, int population_size, int dimension, domain domain_function, int function_number)
{
    DEBUG(printf("\ngenerate_island\n"););
    populacao *populations = malloc(island_size * sizeof(populacao));
    populacao **neighbours = calloc(4, sizeof(populacao *));
    for (int i = 0; i < island_size; i++)
    {
        populations[i].individuos = generate_population(population_size, dimension, domain_function, function_number);
        populations[i].size = population_size;
        populations[i].crossover = rand() % 6;
        populations[i].neighbours = calloc(4, sizeof(populacao *));
        populations[i].neighbours[0] = &populations[(i + 1) % island_size]; // talvez isso dê problema
        populations[i].neighbours[1] = &populations[(i + 3) % island_size]; // talvez isso dê problema
    }
    return populations;
}

individuo *get_best_of_population(populacao populacao)
{
    DEBUG(printf("\nget_best_of_population\n"););
    qsort(populacao.individuos, populacao.size, sizeof(individuo), comparador_individuo);
    return &populacao.individuos[populacao.size - 1];
}

individuo *get_worst_of_population(individuo *population, int n_populacoes)
{
    qsort(population, n_populacoes, sizeof(individuo), comparador_individuo);
    return &population[0];
}

void clone_individue(individuo *clone, individuo *original, int dimension)
{
    for (int i = 0; i < dimension; i++)
    {
        clone->chromosome[i] = original->chromosome[i];
    }
    clone->fitness = original->fitness;
}

void destroy_population(individuo *population, int n_individuos)
{
    DEBUG(printf("\ndestroy_population\n"););
    for (int i = 0; i < n_individuos; i++)
    {
        free(population[i].chromosome);
    }
    free(population);
}

void destroy_island(populacao *populations, int island_size)
{
    DEBUG(printf("\ndestroy_island\n"););
    for (int i = 0; i < island_size; i++)
    {
        destroy_population(populations[i].individuos, populations[i].size);
        free(populations[i].neighbours);
    }
    free(populations);
}