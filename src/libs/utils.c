#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <math.h>

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

int doubleEqual(double a, double b, int num_casas) {
    double tolerance = pow(10, -num_casas);
    return fabs(a - b) < tolerance;
}

double calc_mean(individuo *population, int population_size)
{
    double sum = 0.0;
    for (int i = 0; i < population_size; i++)
    {
        sum += population[i].fitness;
    }
    return sum / population_size;
}

double desvio_padrao(individuo *population, int population_size)
{
    double mean = calc_mean(population, population_size);
    double sum = 0.0;
    for (int i = 0; i < population_size; i++)
    {
        sum += pow(population[i].fitness - mean, 2);
    }
    return sqrt(sum / population_size);
}

void copy_individuo(individuo *original, individuo *copia, int dimension)
{
    copia->fitness = original->fitness;
    for (int i = 0; i < dimension; i++)
    {
        copia->chromosome[i] = original->chromosome[i];
    }
}

char *translateIntToAlg(enum algorithm alg)
{
    switch (alg)
    {
    case PSO:
        return "PSO";
    case GA:
        return "GA";
    case DE:
        return "DE";
    case ACO:
        return "ACO";
    case CLONALG:
        return "CLONALG";
    default:
        return "Invalid algorithm";
    }
}

// Função para imprimir um vetor em formato de lista
void printVector(int arr[], int n)
{
    printf("[");
    for (int i = 0; i < n; i++)
    {
        printf("%s", translateIntToAlg(arr[i] + 1));
        if (i < n - 1)
        {
            printf(", ");
        }
    }
    printf("]");
}

// Função recursiva para gerar combinações e montá-las em um vetor
void generateCombinations(int n, int arr[], int index, int **result, int *size)
{
    if (index == n)
    {
        result[*size] = (int *)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++)
        {
            result[*size][i] = arr[i];
        }
        (*size)++;
        return;
    }

    for (int i = (index == 0) ? 0 : arr[index - 1]; i < n; i++)
    {
        arr[index] = i;
        generateCombinations(n, arr, index + 1, result, size);
    }
}

Array generateComb(int n)
{
    int arr[n];
    int **combinations = (int **)malloc(n * n * n * n * sizeof(int *));
    int size = 0;

    generateCombinations(n, arr, 0, combinations, &size);
    
    return (Array) {combinations, size};
}

/**
 * @brief Converts a string of comma separated numbers to an array of integers
 * @param parameter String of comma separated numbers. Ex: [1,2,3,4,5]
 * @return array of integers
 */
int size_of_array(char *parameter)
{
    int i = 0;
    char parameters_copy[256];
    strcpy(parameters_copy, parameter);
    char *token = strtok(parameters_copy, ",");
    while (token != NULL)
    {
        i++;
        token = strtok(NULL, ",");
    }
    return i;
}

int *convert_parameter_to_array(char *parameter)
{
    char parameters_copy[256];
    strcpy(parameters_copy, parameter);
    int *array = calloc(size_of_array(parameter), sizeof(int));
    int i = 0;
    char *token = strtok(parameters_copy, ",");
    while (token != NULL)
    {
        array[i] = atoi(token);
        i++;
        token = strtok(NULL, ",");
    }
    return array;
}

