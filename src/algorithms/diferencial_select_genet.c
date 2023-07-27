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

static args parameters;

void print_usage()
{
    printf("Usage: ./evolucao_mpop -f <function_number> -t <time_limit> -i <island_size> -p <population_size> -d <dimension> -l <bounds_lower> -u <bounds_upper> -g <num_generations> -m <mutation_probability>");
}

void set_default_parameters()
{
    parameters.F = 0.55481;
    parameters.function_number = 3;
    parameters.time_limit = 10; // seconds
    parameters.island_size = 1;
    parameters.population_size = 142;
    parameters.dimension = 10; // 10 or 30
    parameters.domain_function.min = -100;
    parameters.domain_function.max = 100;
    parameters.mutation_rate = 68;  // %
    parameters.crossover_rate = 99; // %
    parameters.num_migrations = 0;
    parameters.evaluation_limit = 1490400;
    parameters.seed = time(NULL);
}

void set_parameters(int argc, char *argv[])
{
    int opt;
    set_default_parameters();
    while ((opt = getopt(argc, argv, "f:F:t:i:p:d:l:u:g:m:c:k:s:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            parameters.function_number = atoi(optarg);
            break;
        case 't':
            parameters.time_limit = atoi(optarg);
            break;
        case 'i':
            parameters.island_size = atoi(optarg);
            break;
        case 'p':
            parameters.population_size = atoi(optarg);
            break;
        case 'd':
            parameters.dimension = atoi(optarg);
            break;
        case 'l':
            parameters.domain_function.min = atoi(optarg);
            break;
        case 'u':
            parameters.domain_function.max = atoi(optarg);
            break;
        case 'g':
            parameters.num_generations_per_epoca = atoi(optarg);
            break;
        case 'm':
            parameters.mutation_rate = atoi(optarg);
            break;
        case 'c':
            parameters.crossover_rate = atoi(optarg);
            break;
        case 'k':
            parameters.num_migrations = atoi(optarg);
            break;
        case 'F':
            parameters.F = atof(optarg);
            break;
        case 's':
            parameters.seed = atoi(optarg);
            break;
        default:
            printf("Invalid option: %c\n", opt);
            print_usage();
            exit(1);
            break;
        }
    }
}

void fitness(individuo *individuo, int dimension)
{
    // individuo.fitness = real_function(individuo.chromosome, dimension);
    cec15_test_func(individuo->chromosome, &individuo->fitness, dimension, 1, parameters.function_number);
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

individuo *generate_population(int n_individuos, int dimension, domain domain_function)
{
    DEBUG(printf("\ngenerate_population\n"););
    individuo *population = malloc(n_individuos * sizeof(individuo));
    for (int i = 0; i < n_individuos; i++)
    {
        population[i].chromosome = (double *)malloc(dimension * sizeof(double));
        for (int j = 0; j < dimension; j++)
        {
            population[i].chromosome[j] = random_double(domain_function.min, domain_function.max);
        }
        fitness(&population[i], dimension);
    }
    return population;
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
        populations[i].individuos = generate_population(population_size, dimension, domain_function);
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

populacao *mutation_diferencial(populacao *populacao_original, int dimension, domain domain_function)
{
    DEBUG(printf("\nMutation\n"););

    // TODO: populacao_mutada[i] = populacao_original não ocorre em NAO_MUTAR
    populacao *populacao_mutada = generate_island(1, populacao_original->size, dimension, domain_function);

    for (int i = 0; i < populacao_original->size; i++)
    {

        DEBUG(printf("\nIndividuo %d\n", i); print_individuo(populacao_original->individuos[i], dimension, i);)

        DEBUG(printf("individuo %d sofreu mutação\n", i););
        int alpha, beta, gamma;

        for (int j = 0; j < dimension; j++)
        {
            // double result = rand();
            double result = domain_function.max + 1;
            while (result > parameters.domain_function.max || result < domain_function.min)
            {
                do
                {
                    alpha = rand() % populacao_original->size;
                    beta = rand() % populacao_original->size;
                    gamma = rand() % populacao_original->size;
                } while (alpha == beta || alpha == gamma || beta == gamma);
                result = populacao_original->individuos[alpha].chromosome[j] + parameters.F * (fabs(populacao_original->individuos[beta].chromosome[j]) - fabs(populacao_original->individuos[gamma].chromosome[j]));
            }
            populacao_mutada->individuos[i].chromosome[j] = result;
        }
        fitness(&populacao_mutada->individuos[i], dimension);
        DEBUG(printf("Individuo %d\n", i); print_individuo(populacao_mutada->individuos[i], dimension, i);)
    }
    return populacao_mutada;
}

individuo *get_best_of_population(populacao populacao)
{
    DEBUG(printf("\nget_best_of_population\n"););
    return &populacao.individuos[populacao.size - 1];
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

 /*void selection(populacao *population_original, populacao *population_mutation, int dimension)
 {
     DEBUG(printf("\nselection\n"););
     DEBUG(printf("População original\n"););
     DEBUG(print_population(population_original->individuos, population_original->size, dimension, 1););
     DEBUG(printf("População cruzamento\n"););
     DEBUG(print_population(population_mutation->individuos, population_mutation->size, dimension, 1););
     for (int i = 0; i < population_mutation->size; i++)
     {
         if (population_mutation->individuos[i].fitness < population_original->individuos[i].fitness)
             population_original->individuos[i] = population_mutation->individuos[i];
     }
     qsort(population_original->individuos, population_original->size, sizeof(individuo), comparador_individuo);
     DEBUG(printf("População selecionada\n"););
     DEBUG(print_population(population_original->individuos, population_original->size, dimension, 1););
 }*/

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

populacao *crossover(populacao *populacao_original, populacao *populacao_mutada, int dimension)
{
    populacao *nova_populacao = generate_island(1, populacao_original->size, dimension, parameters.domain_function);
    int sigma;
    for (int i = 0; i < populacao_original->size; i++)
    {
        sigma = rand() % parameters.dimension;
        for (int j = 0; j < dimension; j++)
        {
            if (rand() % 100 < parameters.mutation_rate || j == sigma)
                nova_populacao->individuos[i].chromosome[j] = populacao_mutada->individuos[i].chromosome[j];
            else
                nova_populacao->individuos[i].chromosome[j] = populacao_original->individuos[i].chromosome[j];
        }
        fitness(&nova_populacao->individuos[i], dimension);
    }
    return nova_populacao;
}

populacao *diferencial()
{
    DEBUG(printf("\nevolution\n"););
    individuo *parents[2];
    individuo bestIndividuo = {.fitness = INFINITY};
    populacao *populations = generate_island(parameters.island_size, parameters.population_size, parameters.dimension, parameters.domain_function);
    time_t time_init, time_now;
    int evaluation_count = 0;
    int epoca_count = 0;
    double best_ep_ant = bestIndividuo.fitness;
    time(&time_init);
    time(&time_now);
    DEBUG(printf("Iniciando evolucao\n"););

    populacao *original_population = &populations[0];
    populacao *cross_population;
    populacao *mutation_population;
    int generation_count = 0;

    while (evaluation_count < parameters.evaluation_limit && difftime(time_now, time_init) < parameters.time_limit)
    {
        mutation_population = mutation_diferencial(original_population, parameters.dimension, parameters.domain_function);
        cross_population = crossover(original_population, mutation_population, parameters.dimension);
        //selection(original_population, cross_population, parameters.dimension);
        original_population = selection(original_population, cross_population, parameters.dimension);

        // print_individuo(original_population->individuos[original_population->size - 1], dimension, 1);
        LOG(write_population_log(epoca_count, i, generation_count, *original_population, parameters););
        STATISTICS(print_coords(&original_population->individuos[original_population->size - 1], 1, generation_count, parameters.num_generations_per_epoca););
        DEBUG(printf("\nGeração: %d\n", generation_count););
        generation_count++;
        evaluation_count += original_population->size;
        time(&time_now);
    }
    individuo *bestCurrent = get_best_of_population(*original_population);
    // puts("\nMelhor de toda a população:");
    // print_individuo(*bestCurrent, dimension);
    // printf("%lf\n", bestCurrent->fitness);
    if (bestCurrent->fitness < bestIndividuo.fitness)
        bestIndividuo = *bestCurrent;

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

int main(int argc, char *argv[])
{
    set_parameters(argc, argv); // Lê os parâmetros da linha de comando e repassa para as variáveis globais
    // print_parameters();

    populacao *result;
    // Melhor semente até agora: 1676931005 (Funcao 3) - 301.356
    // Melhor semente até agora: 1676935665 (Funcao 8) - 801.1393
    srand(parameters.seed);
    result = diferencial();

    // print_individuo(result, parameters.dimension, 0);
    printf("Best %lf\n", get_best_of_population(*result)->fitness);
    return 0;
}
