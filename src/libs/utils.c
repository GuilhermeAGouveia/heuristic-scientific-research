#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

void free_population(populacao *populations, int n_populations) {
    for (int i = 0; i < n_populations; i++) {
        free(populations[i].individuos);
        free(populations[i].neighbours);
    }
    free(populations);
}

void print_individuo(individuo individuo, int dimension, int id)
{
    DEBUG(printf("\nprint_individuo\n"););
    printf("Individuo %d: ", id);
    for (int j = 0; j < dimension; j++)
    {
        printf("%lf ", individuo.chromosome[j]);
    }
    printf("-> %lf\n", individuo.fitness);
}

void print_population(individuo *pop, int tamanho_populacao, int dimension, int only_fitness)
{
    DEBUG(printf("\nprint_population\n"););
    for (int i = 0; i < tamanho_populacao; i++)
    {
        only_fitness ? printf("%lf ", pop[i].fitness) : print_individuo(pop[i], dimension, i);
    }
    printf("\n");
}


double random_double(double min, double max)
{
    double random = min + (max - min) * (rand() / (double)RAND_MAX);
    return random;
}
