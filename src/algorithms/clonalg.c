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

void print_usage()
{
    printf("Usage: ./evolucao_mpop -f <function_number> -t <time_limit> -i <island_size> -p <population_size> -d <dimension> -l <bounds_lower> -u <bounds_upper> -g <num_generations> -m <mutation_probability>");
}

typedef struct ClonalgArgs_
{
    int population_size;
    int clone_number;
    int dimension;
    int function_number;
    int time_limit;
    int num_generations;
    domain domain_function;
    int seed;
} ClonalgArgs;

static ClonalgArgs parameters;

void set_default_parameters()
{
    parameters.function_number = 3;
    parameters.time_limit = 10; // seconds
    parameters.population_size = 124;
    parameters.dimension = 10; // 10 or 30
    parameters.domain_function.min = -100;
    parameters.domain_function.max = 100;
    parameters.num_generations = 300;
    parameters.clone_number = 36;
    parameters.seed = time(NULL);
}

void set_parameters(int argc, char *argv[])
{
    int opt;
    set_default_parameters();
    while ((opt = getopt(argc, argv, "f:t:p:d:l:u:g:c:s:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            parameters.function_number = atoi(optarg);
            break;
        case 't':
            parameters.time_limit = atoi(optarg);
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
            parameters.num_generations = atoi(optarg);
            break;
        case 'c':
            parameters.clone_number = atoi(optarg);
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

void mutation_n_genes(populacao *populacao_clones, int n_genes, int dimension, domain domain_function)
{
    DEBUG(printf("\nmutation\n"););
    if (n_genes > dimension)
    {
        printf("n_genes > dimension\n");
        exit(1);
    }

    int mutation_point;
    DEBUG(printf("Original:\n"););
    DEBUG(print_individuo(populacao_clones->individuos[0], dimension, 0););
    DEBUG(printf("Mutated %d genes:\n", n_genes););
    for (int i = 0; i < populacao_clones->size; i++)
    {
        int *mutation_positions = calloc(dimension, sizeof(int));
        for (int j = 0; j < n_genes; j++)
        {
            do
            {
                mutation_point = rand() % dimension;
            } while (mutation_positions[mutation_point]);
            mutation_positions[mutation_point] = 1;
            populacao_clones->individuos[i].chromosome[mutation_point] = random_double(domain_function.min, domain_function.max);
        }
        fitness(&populacao_clones->individuos[i], dimension);
        DEBUG(print_individuo(populacao_clones->individuos[i], dimension, i););
        free(mutation_positions);
    }
}

individuo *get_best_of_population(populacao *populacao)
{
    DEBUG(printf("\nget_best_of_population\n"););
    DEBUG(print_population(populacao->individuos, populacao->size, parameters.dimension, 1););
    qsort(populacao->individuos, populacao->size, sizeof(individuo), comparador_individuo);
    DEBUG(printf("Best: %lf\n", populacao->individuos[populacao->size - 1].fitness););
    return &populacao->individuos[populacao->size - 1];
}

void clone_individue(individuo *clone, individuo *original, int dimension)
{
    for (int i = 0; i < dimension; i++)
    {
        clone->chromosome[i] = original->chromosome[i];
    }
    clone->fitness = original->fitness;
}

// log_scale: [c, d] -> [a, b]
// log_scale(c) = a
// log_scale(d) = b
double log_scale(double x, double a, double b, double c, double d)
{
    double result = a + (b - a) * (log(x) - log(c)) / (log(d) - log(c));
    return result;
}

populacao *generate_clones(populacao *population_main, int clone_number, int dimension, domain domain_function)
{
    DEBUG(printf("\ngenerate_clones\n"););
    populacao *populacao_clones = generate_island(population_main->size, clone_number, dimension, domain_function);
    DEBUG(printf("Original:\n"););
    DEBUG(print_population(population_main->individuos, population_main->size, dimension, 1););
    for (int i = 0; i < population_main->size; i++)
    {
        populacao_clones[i].n_mutations = ceil(log_scale(population_main->size - i, 1, dimension, 1, population_main->size));
        populacao_clones[i].size = clone_number;
        for (int j = 0; j < clone_number; j++)
        {
            clone_individue(&populacao_clones[i].individuos[j], &population_main->individuos[i], dimension);
        }
        DEBUG(printf("Clone %d:\n", i););
        DEBUG(print_population(populacao_clones[i].individuos, clone_number, dimension, 1););
    }
    return populacao_clones;
}

populacao *mutation_clones(populacao *populacao_clones, int num_population_clones, int dimension, domain domain_function)
{
    DEBUG(printf("\nmutation_clones\n"););
    for (int i = 0; i < num_population_clones; i++)
    {
        DEBUG(printf("\nmutation_clones %d\n", i););
        mutation_n_genes(&populacao_clones[i], populacao_clones[i].n_mutations, dimension, domain_function);
        DEBUG(print_population(populacao_clones[i].individuos, populacao_clones[i].size, dimension, 1););
    }
    return populacao_clones;
}

void union_populacao_clones_and_main(populacao *populacao_clones, populacao *population_main, int num_population_clones)
{
    DEBUG(printf("\nunion_populacao_clones_and_main\n"););
    DEBUG(printf("Original:\n"););
    DEBUG(print_population(population_main->individuos, population_main->size, parameters.dimension, 1););
    for (int i = 0; i < num_population_clones; i++)
    {
        individuo *best_clone = get_best_of_population(&populacao_clones[i]);
        if (best_clone->fitness < population_main->individuos[i].fitness)
        {
            clone_individue(&population_main->individuos[i], best_clone, parameters.dimension);
        }
    }
}

individuo clonalg()
{
    DEBUG(printf("\nevolution\n"););
    populacao *population_main = generate_island(1, parameters.population_size, parameters.dimension, parameters.domain_function);
    qsort(population_main->individuos, population_main->size, sizeof(individuo), comparador_individuo);
    populacao *populacao_clones;
    populacao *populacao_clones_mutated;
    time_t time_init, time_now;
    int generation_count = 0;
    time(&time_init);
    time(&time_now);
    DEBUG(printf("Iniciando evolucao\n"););
    //  while (difftime(time_now, time_init) < parameters.time_limit)
    // {

    int max_inter_add = 50;
    int max_inter = 100;
    int cont_or_stop = 1;
    while (cont_or_stop && difftime(time_now, time_init) < parameters.time_limit)
    {
        double best_anter = population_main->individuos[population_main->size - 1].fitness;
        while (generation_count < max_inter && difftime(time_now, time_init) < parameters.time_limit)
        {
            STATISTICS(print_coords(&population_main->individuos[population_main->size - 1], 1, generation_count, parameters.num_generations););

            DEBUG(printf("Populacao principal na geracao %d:\n", generation_count););
            DEBUG(print_population(population_main->individuos, population_main->size, parameters.dimension, 1););
            populacao_clones = generate_clones(population_main, parameters.clone_number, parameters.dimension, parameters.domain_function);
            populacao_clones_mutated = mutation_clones(populacao_clones, parameters.population_size, parameters.dimension, parameters.domain_function);
            union_populacao_clones_and_main(populacao_clones_mutated, population_main, parameters.population_size);
            free_population(populacao_clones, parameters.population_size);
            qsort(population_main->individuos, population_main->size, sizeof(individuo), comparador_individuo);
            generation_count++;
            time(&time_now);
        }
        // double desv = desvio_padrao(population_main->individuos, parameters.population_size);
        // printf("Desvio_P: %lf\n", desv);
        // printf("Anterior: %lf, Atual_best:%lf\n", best_anter, population_main->individuos[population_main->size - 1].fitness);

        if (doubleEqual(best_anter, population_main->individuos[population_main->size - 1].fitness, 2)){
            cont_or_stop = 0;
        }
            
        else
        {
            max_inter += max_inter_add;
        }
    }
    return population_main->individuos[population_main->size - 1];
}

int main(int argc, char *argv[])
{
    set_parameters(argc, argv); // Lê os parâmetros da linha de comando e repassa para as variáveis globais
    // print_parameters();

    individuo result;

    srand(parameters.seed);
    result = clonalg();
    // print_individuo(result, parameters.dimension, 0);
    printf("Best %lf\n", result.fitness);
    return 0;
}
