#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "../libs/funcoes_cec_2015/cec15_test_func.h"
#include "../libs/statistics.h"
#include "../libs/types.h"
#include "../libs/utils.h"
#include "../libs/crossover.h"
#define STATISTICS(x) x
#define DEBUG(x)

#define POPULATION_SIZE 100
#define NUM_GENERATIONS 10
#define MUTATION_PROBABILITY 80 // %
#define DIMENSION 10            // 10 or 30
#define BOUNDS_LOWER -100
#define BOUNDS_UPPER 100
#define SELECT_CRITERIA 0.0001
#define FUNCTION_NUMBER 3 // 1 to 15

#define TIME_LIMIT 10 // seconds

static int function_number = FUNCTION_NUMBER;
static int time_limit = TIME_LIMIT;

void fitness(individuo *individuo, int dimension)
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

int avaliar(individuo *populacao, int n_populacoes, int select_criteria)
{
    int count = 0;
    DEBUG(printf("\navaliar\n"););
    for (int i = 0; i < n_populacoes; i++)
    {
        double individuo_custo = populacao[i].fitness;
        if (individuo_custo < select_criteria)
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

int roleta_pais(individuo *populacao, int n_populacoes)
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
        double individuo_beneficio = sum_beneficio - populacao[i].fitness;
        DEBUG(printf("individuo_beneficio: %lf\n", individuo_beneficio););
        int limit = ceil((double)(individuo_beneficio) / (double)sum_beneficio * 100.00);
        DEBUG(printf("Preenchendo roleta com %d de %d até %d\n", i, base, base + limit););
        for (int j = base; j < base + limit; j++)
        {
            roulette[j] = i;
        }
        base += limit;
    }

    return roulette[rand() % base];
}

void select_parents(individuo *populacao, int n_populacoes, individuo *parents[2])
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

individuo cruzamento(individuo *parents[2], int n_itens)
{
    DEBUG(printf("\ncruzamento\n"););
    individuo parent1 = *parents[0];
    individuo parent2 = *parents[1];
    int crossover = PONTO;
    individuo filho;
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

int in_fitness_population(individuo *populacao, int n_populacoes, individuo individuo)
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

individuo *generate_population(int n_populacoes, int dimension, domain domain_function)
{
    DEBUG(printf("\ngenerate_population\n"););
    individuo *population = malloc(n_populacoes * sizeof(individuo));
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

individuo mutation(individuo *population, individuo individuo, int dimension, domain domain_function)
{
    DEBUG(printf("\nmutation\n"););
    int alpha, beta, gamma;
    do
    {
        alpha = rand() % dimension;
        beta = rand() % dimension;
        gamma = rand() % dimension;
    } while (alpha == beta || alpha == gamma || beta == gamma);
    for (int i = 0; i < dimension; i++)
    {
        individuo.chromosome[i] = population[alpha].chromosome[i] + 0.8 * (population[beta].chromosome[i] - population[gamma].chromosome[i]);
    }
    return individuo;
}

individuo *get_best_of_population(individuo *population, int n_populacoes)
{
    qsort(population, n_populacoes, sizeof(individuo), comparador_individuo);
    return &population[n_populacoes - 1];
}

int selection(individuo *population, int n_populacoes, int dimension)
{
    DEBUG(printf("\nselection\n"););
    for (int i = 0; i < n_populacoes; i++)
    {
        if (population[i].fitness == INFINITY)
            fitness(&population[i], dimension);
    }

    qsort(population, n_populacoes, sizeof(individuo), comparador_individuo);
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

individuo *get_pior_pai(individuo *pais[2])
{
    return pais[0]->fitness < pais[1]->fitness ? pais[1] : pais[0];
}

individuo *evolution(int population_size, int dimension, domain domain_function, double select_criteria, int num_generations)
{
    DEBUG(printf("\nevolution\n"););
    individuo *parents[2];
    individuo *population = generate_population(population_size, dimension, domain_function);
    // int generations_count = 0;
    time_t time_init, time_now;
    int generations_count = 0;
    time(&time_init);
    int max_inter = 300;
    int cont_or_stop = 1;
    double aux, best_anter = get_best_of_population(population, population_size)->fitness;
    while (cont_or_stop)
    {
        do
        {
            // printf("\ni-ésima geração: %d\n", generations_count);
            selection(population, population_size, dimension);
            // print_population(population, population_size, dimension);
            for (int i = 0; i < population_size - 1; i++)
            {
                DEBUG(printf("\ni-ésimo individuo: %d\n", i););

                select_parents(population, population_size, parents);
                individuo child = cruzamento(parents, dimension);
                // O if abaixo garante que nunca haverá dois individuos iguais na população
                if (in_fitness_population(population, population_size, child))
                {
                    child = mutation(population, child, dimension, domain_function);
                }
                DEBUG(printf("Custo do filho: %lf\n", child.fitness););
                individuo *pior_pai = get_pior_pai(parents);
                *pior_pai = child;

                if (rand() % 100 < MUTATION_PROBABILITY)
                {
                    population[i] = mutation(population, population[i], dimension, domain_function);
                }
            }
            time(&time_now);
            generations_count++;
            STATISTICS(print_coords(population, population_size, generations_count, num_generations););

            // printf("Geração: %d\n", generations_count);
        } while (!avaliar(population, population_size, select_criteria) && difftime(time_now, time_init) < time_limit && generations_count < max_inter);
        aux = get_best_of_population(population, population_size)->fitness;
        if (best_anter == aux)
            cont_or_stop = 0;
        else
            best_anter = aux;
    }

    return get_best_of_population(population, population_size);
}

int main(int argc, char *argv[])
{
    individuo *result = NULL;
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
    print_individuo(*result, 10, 1);
    return 0;
}