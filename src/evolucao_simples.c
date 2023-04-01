#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "./libs/funcoes_cec_2015/cec15_test_func.h"
#include "./libs/statistics.h"
#include "./libs/types.h"
#include "./libs/utils.h"
#include "./libs/crossover.h"
#define STATISTICS(x) x
#define DEBUG(x)

#define POPULATION_SIZE 100
#define NUM_GENERATIONS 1000
#define MUTATION_PROBABILITY 80 // %
#define DIMENSION 10            // 10 or 30
#define BOUNDS_LOWER -100
#define BOUNDS_UPPER 100
#define SELECT_CRITERIA 0.0001
#define FUNCTION_NUMBER 1 // 1 to 15

#define TIME_LIMIT 10 // seconds

static int function_number = FUNCTION_NUMBER;
static int time_limit = TIME_LIMIT;

void fitness(individue *individuo, int dimension)
{
    // individuo.fitness = real_function(individuo.chromosome, dimension);
    cec15_test_func(individuo->chromosome, &individuo->fitness, dimension, 1, function_number);
    // double x = individuo->chromosome[0];
    // double y = individuo->chromosome[1];
    // individuo->fitness = pow(x, 2) + pow(y, 2) - cos(18 * x) - cos(18 * y);
    // printf("fitness: %f\n", individuo->fitness);
}

int comparador_individue(const void *a, const void *b)
{
    individue *v1 = (individue *)a;
    individue *v2 = (individue *)b;

    return v1->fitness < v2->fitness;
}

int avaliar(individue *populacao, int n_populacoes, int select_criteria)
{
    int count = 0;
    DEBUG(printf("\navaliar\n"););
    for (int i = 0; i < n_populacoes; i++)
    {
        double individue_custo = populacao[i].fitness;
        if (individue_custo < select_criteria)
        {
            count += 1;
        }
    }

    float percent = (float)count / (float)n_populacoes;
    return percent > 0.90;
}

void print_roleta(int *roleta, int roleta_size, int ball1, int ball2)
{
    DEBUG(printf("\nprint_roleta\n"););
    for (int i = 0; i < roleta_size; i++)
    {
        if (i == ball1 || i == ball2)
            printf("%d<-o ", roleta[i]);
        else
            printf("%d ", roleta[i]);
    }
    printf("\n\n");
}

int roleta_pais(individue *populacao, int n_populacoes)
{
    DEBUG(printf("\nselect_parents\n"););
    int roulette[100000];
    double sum_beneficio = 0.0;

    DEBUG(printf("sum_beneficio[init]\n"););
    for (int i = 0; i < n_populacoes; i++)
    {
        sum_beneficio += populacao[i].fitness;
    }
    DEBUG(printf("sum_beneficio[end]: %lf\n", sum_beneficio););

    int base = 0;
    for (int i = 0; i < n_populacoes; i++)
    {
        double individue_beneficio = sum_beneficio - populacao[i].fitness;
        DEBUG(printf("individue_beneficio: %lf\n", individue_beneficio););
        int limit = ceil((double)(individue_beneficio) / (double)sum_beneficio * 100.00);
        DEBUG(printf("Preenchendo roleta com %d de %d até %d\n", i, base, base + limit););
        for (int j = base; j < base + limit; j++)
        {
            roulette[j] = i;
        }
        base += limit;
    }

    return roulette[rand() % base];
}

void select_parents(individue *populacao, int n_populacoes, individue *parents[2])
{
    int pai1 = roleta_pais(populacao, n_populacoes), pai2;
    do
    {
        pai2 = roleta_pais(populacao, n_populacoes);
    } while (pai1 == pai2);
    parents[0] = &populacao[pai1];
    parents[1] = &populacao[pai2];
    // printf("Pais: %d e %d\n", pai1, pai2);
}

individue cruzamento(individue *parents[2], int n_itens)
{
    DEBUG(printf("\ncruzamento\n"););
    individue parent1 = *parents[0];
    individue parent2 = *parents[1];
    int crossover = PONTO;
    individue filho;
    switch (crossover)
    {
    case MEDIA:
        filho = cruzamento_media(parent1, parent2, n_itens);
    case METADE:
        filho = cruzamento_metade(parent1, parent2, n_itens);
    case PONTO:
        filho = cruzamento_ponto(parent1, parent2, n_itens);
    case MEDIA_GEOMETRICA:
        filho = cruzamento_ponto(parent1, parent2, n_itens);
    case FLAT:
        filho = cruzamento_flat(parent1, parent2, n_itens);
    case BLEND:
        filho = cruzamento_blend(parent1, parent2, n_itens);
    default:
        filho = cruzamento_media(parent1, parent2, n_itens);
    }

    fitness(&filho, n_itens);
    return filho;
}

int in_fitness_population(individue *populacao, int n_populacoes, individue individuo)
{
    DEBUG(printf("\nin_fitness_population\n"););
    for (int i = 0; i < n_populacoes; i++)
    {
        if (populacao[i].fitness == individuo.fitness)
        {
            return 1;
        }
    }
    return 0;
}

individue *generate_population(int n_populacoes, int dimension, domain domain_function)
{
    DEBUG(printf("\ngenerate_population\n"););
    individue *population = malloc(n_populacoes * sizeof(individue));
    for (int i = 0; i < n_populacoes; i++)
    {
        population[i].chromosome = (double *)malloc(dimension * sizeof(double));
        for (int j = 0; j < dimension; j++)
        {
            population[i].chromosome[j] = random_double(domain_function.min, domain_function.max);
        }
        population[i].fitness = INFINITY;
    }

    // DEBUG(print_population(population, n_populacoes, dimension););
    return population;
}

individue mutation(individue individuo, int dimension, domain domain_function)
{
    DEBUG(printf("\nmutation\n"););
    int mutation_point = rand() % dimension;
    individuo.chromosome[mutation_point] = random_double(domain_function.min, domain_function.max);
    // individuo.chromosome[mutation_point] = !individuo.chromosome[mutation_point];
    fitness(&individuo, dimension);

    return individuo;
}

individue *get_best_of_population(individue *population, int n_populacoes)
{
    qsort(population, n_populacoes, sizeof(individue), comparador_individue);
    return &population[n_populacoes - 1];
}

int selection(individue *population, int n_populacoes, int dimension)
{
    DEBUG(printf("\nselection\n"););
    for (int i = 0; i < n_populacoes; i++)
    {
        if (population[i].fitness == INFINITY)
            fitness(&population[i], dimension);
    }

    qsort(population, n_populacoes, sizeof(individue), comparador_individue);
    return 0;
}

/**
 * @brief Algoritmo Evolucionário
 * O algoritmo para quando:
 * 1. Cerca de 90% da população for avaliada como melhor do que o critério de seleção
 * 2. O tempo de execução for maior que 10 segundos
 * 3. O numero de gerações for 500
 * @param itens
 * @param n_itens
 * @param capacidad
 * @param population_size
 * @param select_criteria
 * @return int*
 */

individue *get_pior_pai(individue *pais[2])
{
    return pais[0]->fitness < pais[1]->fitness ? pais[1] : pais[0];
}

individue *evolution(int population_size, int dimension, domain domain_function, double select_criteria, int num_generations)
{
    DEBUG(printf("\nevolution\n"););
    individue *parents[2];
    individue *population = generate_population(population_size, dimension, domain_function);
    // int generations_count = 0;
    time_t time_init, time_now;
    int generations_count = 0;
    time(&time_init);
    do
    {
        // printf("\ni-ésima geração: %d\n", generations_count);
        selection(population, population_size, dimension);
        // print_population(population, population_size, dimension);
        for (int i = 0; i < population_size - 1; i++)
        {

                mutation_diferencial(current_population, dimension, domain_function);
                cross_population = crossover(current_population, dimension);
                *current_population = selection(current_population, cross_population, dimension);
                
        time(&time_now);
        generations_count++;
        STATISTICS(print_coords(population, population_size, generations_count, num_generations););

        // printf("Geração: %d\n", generations_count);
    } while (!avaliar(population, population_size, select_criteria) && difftime(time_now, time_init) < time_limit && generations_count < num_generations);

    return get_best_of_population(population, population_size);
}

int main(int argc, char *argv[])
{
    individue *result = NULL;
    time_t semente = time(NULL);
    printf("Semente: %ld\n ", semente);
    srand(semente);
    if (argc < 2)
        result = evolution(POPULATION_SIZE, DIMENSION, (domain){BOUNDS_LOWER, BOUNDS_UPPER}, SELECT_CRITERIA, NUM_GENERATIONS);
    else
    {
        function_number = atoi(argv[1]);
        time_limit = atoi(argv[2]);
        result = evolution(POPULATION_SIZE, DIMENSION, (domain){BOUNDS_LOWER, BOUNDS_UPPER}, SELECT_CRITERIA, NUM_GENERATIONS);
    }
    print_individue(*result, 10);
    return 0;
}