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
#include "parameters.h"
#include "commom.h"
#define STATISTICS(x)
#define DEBUG(x)
#define LOG(x)

void set_default_parameters_genetic()
{

    if (!parameters.function_number)
        parameters.function_number = 3;
    if (!parameters.num_generations_per_epoca)
        parameters.num_generations_per_epoca = 1226;
    if (!parameters.time_limit)
        parameters.time_limit = 10; // seconds
    if (!parameters.population_size)
        parameters.population_size = 1025; // 507;
    if (!parameters.dimension)
        parameters.dimension = 10; // 10 or 30
    if (!parameters.domain_function.min)
        parameters.domain_function.min = -100;
    if (!parameters.domain_function.max)
        parameters.domain_function.max = 100;
    if (!parameters.mutation_rate)
        parameters.mutation_rate = 13; // 6; // %
    if (!parameters.crossover_rate)
        parameters.crossover_rate = 33; // 21; // %
    if (!parameters.evaluation_limit)
        parameters.evaluation_limit = 1490400;
    if (!parameters.seed)
        parameters.seed = time(NULL);
    srand(parameters.seed);
}

void reset_parameters_genetic()
{
    parameters.population_size = 0;
    parameters.mutation_rate = 0;
    parameters.crossover_rate = 0;
    parameters.seed = 0;
    parameters.num_generations_per_epoca = 0;
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

int roleta_pais(individuo *populacao, int num_individuos)
{
    // DEBUG(printf("\nroleta\n"););
    int roulette[num_individuos + 100];
    double sum_beneficio = 0.0;
    double sum_beneficio_inv = 0.0;

    // DEBUG(printf("sum_beneficio[init]\n"););
    for (int i = 0; i < num_individuos; i++)
    {
        sum_beneficio += populacao[i].fitness;
    }

    for (int i = 0; i < num_individuos; i++)
    {
        sum_beneficio_inv += sum_beneficio - populacao[i].fitness;
    }
    // DEBUG(printf("sum_beneficio[end]: %lf\n", sum_beneficio););

    int base = 0;
    for (int i = 0; i < num_individuos; i++)
    {
        double individuo_beneficio_inv = sum_beneficio - populacao[i].fitness;
        // DEBUG(printf("individuo_beneficio: %lf\n", populacao[i].fitness););
        int limit = ceil((individuo_beneficio_inv / sum_beneficio_inv) * 100.00);
        // DEBUG(printf("Preenchendo roleta com %d de %d até %d\n", i, base, base + limit););
        for (int j = base; j < base + limit; j++)
        {
            roulette[j] = i;
        }
        base += limit;
    }
    // DEBUG(printf("Numero de elementos na rolêta: %d\n", base););
    // DEBUG(printf("Indice sorteado: %d\n", roulette[rand() % base]););
    return roulette[rand() % base];
}

void select_parents(populacao populacao, individuo *parents[2])
{
    DEBUG(printf("\nselect_parents\n"););
    int pai1 = roleta_pais(populacao.individuos, populacao.size), pai2;
    do
    {
        pai2 = roleta_pais(populacao.individuos, populacao.size);
    } while (pai1 == pai2);
    parents[0] = &populacao.individuos[pai1];
    parents[1] = &populacao.individuos[pai2];

    DEBUG(printf("Pais: %d e %d\n", pai1, pai2););
}

populacao *mutation_commom(populacao *populacao, int dimension, domain domain_function)
{
    DEBUG(printf("\nmutation\n"););
    for (int i = 0; i < populacao->size - 1; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            if (rand() % 100 < parameters.mutation_rate)
            {
                populacao->individuos[i].chromosome[j] = random_double(domain_function.min, domain_function.max);
            }
        }
        fitness(&populacao->individuos[i], dimension, parameters.function_number);
    }
    return populacao;
}

populacao *union_populations(populacao *populacao1, populacao *populacao2)
{
    DEBUG(printf("\nunion_populations\n"););
    int size = populacao1->size + populacao2->size;
    populacao *populacao_unida = generate_island(1, size, parameters.dimension, parameters.domain_function, parameters.function_number);
    for (int i = 0; i < populacao1->size; i++)
    {
        clone_individue(&populacao_unida->individuos[i], &populacao1->individuos[i], parameters.dimension);
    }
    for (int i = 0; i < populacao2->size; i++)
    {
        clone_individue(&populacao_unida->individuos[i + populacao1->size], &populacao2->individuos[i], parameters.dimension);
    }
    return populacao_unida;
}

populacao *slice_population(populacao *population, int start, int end)
{
    DEBUG(printf("\nslice_population [%d, %d]\n", start, end););
    populacao *sliced_population = generate_island(1, end - start, parameters.dimension, parameters.domain_function, parameters.function_number);
    for (int i = 0; i < end - start; i++)
    {
        clone_individue(&sliced_population->individuos[i], &population->individuos[i + start], parameters.dimension);
    }
    return sliced_population;
}

populacao *selection(populacao *population_original, populacao *population_crossover, int dimension)
{
    DEBUG(printf("\nselection\n"););
    DEBUG(printf("População original\n"););
    DEBUG(print_population(population_original->individuos, population_original->size, dimension, 1););
    DEBUG(printf("População cruzamento\n"););
    DEBUG(print_population(population_crossover->individuos, population_crossover->size, dimension, 1););
    populacao *population_union = union_populations(population_original, population_crossover);
    qsort(population_union->individuos, population_union->size, sizeof(individuo), comparador_individuo);
    int original_size = population_original->size;

    destroy_island(population_original, 1);

    *population_original = *slice_population(population_union, population_union->size - original_size, population_union->size);

    destroy_island(population_union, 1);
    destroy_island(population_crossover, 1);

    return population_original;
    DEBUG(printf("População selecionada\n"););
    DEBUG(print_population(population_original->individuos, population_original->size, dimension, 1););
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

populacao *crossover(populacao *populacao_original, populacao *populacao_mutada, int dimension)
{
    populacao *nova_populacao = generate_island(1, populacao_original->size, dimension, parameters.domain_function, parameters.function_number);
    DEBUG(printf("\ncruzamento\n"););
    int i;
    individuo *filho = generate_population(1, parameters.dimension, parameters.domain_function, parameters.function_number);

    for (i = 0; i < populacao_original->size; i++)
    {
        individuo *parents[2];
        if (rand() % 100 < parameters.crossover_rate)
        {
            if (0) // sempre escolher da população original
                select_parents(*populacao_mutada, parents);
            else
                select_parents(*populacao_original, parents);

            individuo parent1 = *parents[0];
            individuo parent2 = *parents[1];
            switch (3) // sempre usar o cruzamento de ponto de corte
            {
            case MEDIA:
                cruzamento_media(parent1, parent2, dimension, filho);
            case METADE:
                cruzamento_metade(parent1, parent2, dimension, filho);
            case PONTO:
                cruzamento_ponto(parent1, parent2, dimension, filho);
            case MEDIA_GEOMETRICA:
                cruzamento_ponto(parent1, parent2, dimension, filho);
            case FLAT:
                cruzamento_flat(parent1, parent2, dimension, filho);
            case BLEND:
                cruzamento_blend(parent1, parent2, dimension, filho);
            default:
                cruzamento_media(parent1, parent2, dimension, filho);
            }

            fitness(filho, dimension, parameters.function_number);
            copy_individuo(filho, &nova_populacao->individuos[i], 10);
        
        }
        else
        {
            clone_individue(&nova_populacao->individuos[i], &populacao_original->individuos[i], dimension);
        }
    }
    destroy_population(filho, 1);
    // nova_populacao->individuos[populacao->size - 1] = populacao->individuos[populacao->size - 1];
    return nova_populacao;
}

populacao *genetic(populacao *population)
{
    set_default_parameters_genetic();
    // print_parameters(parameters);

    if (population == NULL)
    {
        population = generate_island(1, parameters.population_size, parameters.dimension, parameters.domain_function, parameters.function_number);
    }
    DEBUG(printf("\nevolution\n"););
    individuo *parents[2];
    individuo bestIndividuo = {.fitness = INFINITY};
    time_t time_init, time_now;
    int evaluation_count = 0;
    int epoca_count = 0;
    // double best_ep_ant = bestIndividuo.fitness;
    time(&time_init);
    time(&time_now);
    DEBUG(printf("Iniciando evolucao\n"););

    populacao *original_population = population;
    populacao *cross_population;
    populacao *mutation_population;
    int generation_count = 0;

    while (generation_count < parameters.num_generations_per_epoca && difftime(time_now, time_init) < parameters.time_limit)
    {

        cross_population = crossover(original_population, mutation_population, parameters.dimension);
        cross_population = mutation_commom(cross_population, parameters.dimension, parameters.domain_function);
        original_population = selection(original_population, cross_population, parameters.dimension);

        // print_individuo(original_population->individuos[original_population->size - 1], dimension, 1);
        LOG(write_population_log(epoca_count, i, generation_count, *original_population, parameters););
        STATISTICS(print_coords(&original_population->individuos[original_population->size - 1], 1, generation_count, parameters.num_generations_per_epoca););
        DEBUG(printf("\nGeração: %d\n", generation_count););
        generation_count++;
        evaluation_count += original_population->size;
        time(&time_now);
    }

    reset_parameters_genetic();
    return original_population;
}