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

void set_default_parameters()
{
    if (parameters.algorithm != GA)
    {
        printf("Invalid algorithm. Please use GA\n");
        exit(1);
    };
    if (!parameters.function_number)
        parameters.function_number = 3;
    if (!parameters.time_limit)
        parameters.time_limit = 10; // seconds
    if (!parameters.island_size)
        parameters.island_size = 1;
    if (!parameters.population_size)
        parameters.population_size = 100;
    if (!parameters.dimension)
        parameters.dimension = 10; // 10 or 30
    if (!parameters.domain_function.min)
        parameters.domain_function.min = -100;
    if (!parameters.domain_function.max)
        parameters.domain_function.max = 100;
    if (!parameters.mutation_rate)
        parameters.mutation_rate = 25; // %
    if (!parameters.crossover_rate)
        parameters.crossover_rate = 62; // %
    if (!parameters.num_migrations)
        parameters.num_migrations = 0;
    if (!parameters.evaluation_limit)
        parameters.evaluation_limit = 1490400;
    if (!parameters.seed)
        parameters.seed = time(NULL);
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

void destroy_population(individuo *population, int n_individuos)
{
    DEBUG(printf("\ndestroy_population\n"););
    for (int i = 0; i < n_individuos; i++)
    {
        free(population[i].chromosome);
    }
    free(population);
}

populacao *generate_island(int island_size, int population_size, int dimension, domain domain_function)
{
    DEBUG(printf("\ngenerate_island\n"););
    populacao *populations = malloc(island_size * sizeof(populacao));
    populacao **neighbours = calloc(4, sizeof(populacao *));
    for (int i = 0; i < island_size; i++)
    {
        populations[i].individuos = generate_population(population_size, dimension, domain_function, parameters.function_number);
        populations[i].size = population_size;
        populations[i].crossover = rand() % 6;
        populations[i].neighbours = calloc(4, sizeof(populacao *));
        populations[i].neighbours[0] = &populations[(i + 1) % island_size]; // talvez isso dê problema
        populations[i].neighbours[1] = &populations[(i + 3) % island_size]; // talvez isso dê problema
    }
    return populations;
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

populacao *mutation_commom(populacao *populacao, int dimension, domain domain_function)
{
    DEBUG(printf("\nmutation\n"););
    for (int i = 0; i < populacao->size - 1; i++)
    {
        int mutation_point = rand() % dimension;
        populacao->individuos[i].chromosome[mutation_point] = random_double(domain_function.min, domain_function.max);
        fitness(&populacao->individuos[i], dimension, parameters.function_number);
    }
    return populacao;
}

populacao *mutation_diferencial(populacao *populacao_original, int dimension, domain domain_function)
{
    DEBUG(printf("\nMutation\n"););
    populacao *populacao_mutada = generate_island(1, populacao_original->size, dimension, domain_function);

    for (int i = 0; i < populacao_original->size; i++)
    {

        DEBUG(printf("\nIndividuo %d\n", i); print_individuo(populacao_original->individuos[i], dimension, i);)
        if (rand() % 100 > parameters.mutation_rate)
        {
            DEBUG(printf("individuo %d não sofreu mutação\n", i););
            continue;
        }

        DEBUG(printf("individuo %d sofreu mutação\n", i););
        int alpha, beta, gamma;

        do
        {
            alpha = rand() % populacao_original->size;
            beta = rand() % populacao_original->size;
            gamma = rand() % populacao_original->size;
        } while (alpha == beta || alpha == gamma || beta == gamma);

        for (int j = 0; j < dimension; j++)
        {
            // double result = rand();
            double result = populacao_original->individuos[alpha].chromosome[j] + parameters.F * (fabs(populacao_original->individuos[beta].chromosome[j]) - fabs(populacao_original->individuos[gamma].chromosome[j]));
            populacao_mutada->individuos[i].chromosome[j] = result;
        }
        fitness(&populacao_mutada->individuos[i], dimension, parameters.function_number);
        DEBUG(printf("Individuo %d\n", i); print_individuo(populacao_mutada->individuos[i], dimension, i);)
    }
    return populacao_mutada;
}

void clone_individue(individuo *clone, individuo *original, int dimension)
{
    for (int i = 0; i < dimension; i++)
    {
        clone->chromosome[i] = original->chromosome[i];
    }
    clone->fitness = original->fitness;
}

populacao *union_populations(populacao *populacao1, populacao *populacao2)
{
    DEBUG(printf("\nunion_populations\n"););
    int size = populacao1->size + populacao2->size;
    populacao *populacao_unida = generate_island(1, size, parameters.dimension, parameters.domain_function);
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
    populacao *sliced_population = generate_island(1, end - start, parameters.dimension, parameters.domain_function);
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
    populacao *nova_populacao = generate_island(1, populacao_original->size, dimension, parameters.domain_function);
    DEBUG(printf("\ncruzamento\n"););
    int i;

    for (i = 0; i < populacao_original->size; i++)
    {
        individuo *parents[2];
        if (0)
            select_parents(*populacao_mutada, parents);
        else
            select_parents(*populacao_original, parents);

        individuo parent1 = *parents[0];
        individuo parent2 = *parents[1];
        individuo filho;
        switch (populacao_original->crossover)
        {
        case MEDIA:
            filho = cruzamento_media(parent1, parent2, dimension);
        case METADE:
            filho = cruzamento_metade(parent1, parent2, dimension);
        case PONTO:
            filho = cruzamento_ponto(parent1, parent2, dimension);
        case MEDIA_GEOMETRICA:
            filho = cruzamento_ponto(parent1, parent2, dimension);
        case FLAT:
            filho = cruzamento_flat(parent1, parent2, dimension);
        case BLEND:
            filho = cruzamento_blend(parent1, parent2, dimension);
        default:
            filho = cruzamento_media(parent1, parent2, dimension);
        }

        fitness(&filho, dimension, parameters.function_number);
        nova_populacao->individuos[i] = filho;
    }
    // nova_populacao->individuos[populacao->size - 1] = populacao->individuos[populacao->size - 1];
    return nova_populacao;
}

void migrate(populacao *populations, int island_size, int dimension, domain domain_function)
{
    DEBUG(printf("\nmigrate\n"););
    populacao *vizinho;
    // print_population(populations[0].individuos, populations[0].size, dimension, 1);

    for (int i = 0; i < island_size; i++)
    {
        DEBUG(printf("Populacao %d\n", i));
        populacao current_island = populations[i];
        individuo *population = current_island.individuos;
        for (int k = 1; k <= parameters.num_migrations; k++)
        {
            individuo *melhor_individuo_da_populacao = &population[current_island.size - k];
            DEBUG(printf("Melhor individuo da populacao %d: %lf\n", i, melhor_individuo_da_populacao->fitness););
            for (int j = 0; j < 4; j++)
            {
                vizinho = current_island.neighbours[j];
                if (vizinho == NULL)
                    continue;
                individuo *neighbour_population = vizinho->individuos;
                individuo *pior_indivuduo_do_vizinho = get_worst_of_population(neighbour_population, parameters.population_size);
                if (melhor_individuo_da_populacao->fitness > pior_indivuduo_do_vizinho->fitness)
                {
                    individuo *new_worst = generate_population(1, dimension, domain_function, parameters.function_number);
                    *pior_indivuduo_do_vizinho = *new_worst;
                }
                else
                    *pior_indivuduo_do_vizinho = *melhor_individuo_da_populacao;
            }
        }
    }
}

void swap_individuo(individuo *a, individuo *b)
{
    individuo temp = *a;
    *a = *b;
    *b = temp;
}

void shuffle(individuo *pool, int size_pool)
{
    for (int i = 0; i < size_pool; i++)
        swap_individuo(&pool[rand() % size_pool], &pool[rand() % size_pool]);
}

void random_random_migrate(populacao *populations, int island_size, int dimension, domain domain_function)
{
    DEBUG(printf("\nrandom random migrate\n"););
    individuo *pool = generate_population(parameters.num_migrations * island_size, dimension, domain_function, parameters.function_number);
    int positions[island_size][parameters.num_migrations];
    for (int i = 0; i < island_size; i++)
    {
        DEBUG(printf("população %d\n", i););
        for (int j = 0; j < parameters.num_migrations; j++)
        {
            int rand_index = rand() % parameters.population_size;
            DEBUG(printf("posição sorteada %d\n", rand_index););
            if (!populations[i].individuos[rand_index].chromosome)
            { // se for nulo que dizer que o elemento já foi sorteado
                j--;
                DEBUG(printf("posição já foi sorteada\n"););
                continue;
            }

            pool[i * parameters.num_migrations + j] = populations[i].individuos[rand_index]; // corre o risco de sortear o mesmo individuo
            populations[i].individuos[rand_index] = (individuo){NULL, INFINITY, NULL};
            positions[i][j] = rand_index;
        }
    }

    shuffle(pool, parameters.num_migrations * island_size);

    for (int i = 0; i < island_size; i++)
    {
        for (int j = 0; j < parameters.num_migrations; j++)
        {
            populations[i].individuos[positions[i][j]] = pool[i * parameters.num_migrations + j];
        }
    }
}

populacao *genetic()
{
    set_default_parameters();
    print_parameters(parameters);
    DEBUG(printf("\nevolution\n"););
    individuo *parents[2];
    // individuo bestIndividuo = {.fitness = INFINITY};
    populacao *populations = generate_island(parameters.island_size, parameters.population_size, parameters.dimension, parameters.domain_function);
    time_t time_init, time_now;
    int evaluation_count = 0;
    int epoca_count = 0;
    // double best_ep_ant = bestIndividuo.fitness;
    time(&time_init);
    time(&time_now);
    DEBUG(printf("Iniciando evolucao\n"););

    populacao *original_population = &populations[0];
    populacao *cross_population;
    populacao *mutation_population;
    int generation_count = 0;

    while (evaluation_count < parameters.evaluation_limit && difftime(time_now, time_init) < parameters.time_limit)
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
    // individuo *bestCurrent = get_best_of_population(*original_population);
    //  puts("\nMelhor de toda a população:");
    //  print_individuo(*bestCurrent, dimension);
    //  printf("%lf\n", bestCurrent->fitness);
    // if (bestCurrent->fitness < bestIndividuo.fitness)
    //  bestIndividuo = *bestCurrent;

    // migrate(populations, parameters.island_size, dimension, domain_function);

    // Verifica se um best_Individuo foi encontrado em relaçao a epoca anterior
    //  if(doubleEqual(bestIndividuo.fitness, best_ep_ant, 2)){
    //      total_epocs_s_m++;
    //  }
    //  else
    //    total_epocs_s_m = 0;
    //  //Se o limite de epocas sem melhora for atingido é finalizada a evolucao
    //  if(total_epocs_s_m == limit_epocs)
    //     continue_evol = 0;

    return original_population;
}