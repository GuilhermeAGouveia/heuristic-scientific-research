#include <getopt.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "utils.h"
#include "parameters.h"

void print_usage()
{
    printf("Usage: ./evolucao_mpop -f <function_number> -t <time_limit> -i <island_size> -p <population_size> -d <dimension> -l <bounds_lower> -u <bounds_upper> -g <num_generations> -m <mutation_probability> -k");
}

args parameters;

void set_parameters(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "A:f:F:t:i:p:d:l:u:g:m:c:k:s:v:z:C:P:e:K:M:Q:O:D:L:G:N:Z:n:")) != -1)
    {
        switch (opt)
        {
        case 'A':
            parameters.num_algorithms = size_of_array(optarg);
            strcpy(parameters.algorithms, optarg);
            break;
        case 'N':
            parameters.num_algorithms = atoi(optarg);
            break;
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
        case 'K':
            parameters.num_epocas = atoi(optarg);
            break;
        case 'M':
            parameters.choice_random_migrate = atof(optarg);
            break;
        case 'F':
            parameters.F = atof(optarg);
            break;
        case 's':
            parameters.seed = atoi(optarg); // não funciona momento, os algoritmos vão definir uma semente própria
            break;
        case 'v':
            parameters.c1 = atof(optarg);
            break;
        case 'z':
            parameters.c2 = atof(optarg);
            break;
        case 'C':
            parameters.clones = atoi(optarg);
            break;
        case 'e':
            parameters.tax_evaporate = atof(optarg);
            break;
        case 'n':
            parameters.num_candidates = atof(optarg);
            break;
        case 'Q':
            parameters.num_pso = atoi(optarg);
            break;
        case 'L':
            parameters.num_clonal = atoi(optarg);
            break;
        case 'O':
            parameters.num_aco = atoi(optarg);
            break;
        case 'G':
            parameters.num_genetico = atoi(optarg);
            break;
        case 'D':
            parameters.num_diferencial = atoi(optarg);
            break;
        case 'P':
            parameters.p_exploitation = atof(optarg);
            break;
        case 'Z':
            strcpy(parameters.temporary_folder, optarg);
            break;
        default:
            printf("Invalid option: %c\n", opt);
            print_usage();
            exit(1);
            break;
        }
    }
    srand(parameters.seed);
}

void print_parameters(args parameters)
{
    printf("Function number: %d\n", parameters.function_number);
    printf("Time limit: %d\n", parameters.time_limit);
    printf("Island size: %d\n", parameters.island_size);
    printf("Population size: %d\n", parameters.population_size);
    printf("Dimension: %d\n", parameters.dimension);
    printf("Domain function: %lf %lf\n", parameters.domain_function.min, parameters.domain_function.max);
    printf("Number of generations per epoca: %d\n", parameters.num_generations_per_epoca);
    printf("Mutation rate: %d\n", parameters.mutation_rate);
    printf("Crossover rate: %d\n", parameters.crossover_rate);
    printf("Number of migrations: %d\n", parameters.num_migrations);
    printf("F: %f\n", parameters.F);
    printf("Seed: %d\n", parameters.seed);
    printf("C1: %f\n", parameters.c1);
    printf("C2: %f\n", parameters.c2);
    printf("Clones: %d\n", parameters.clones);
    printf("Tax evaporate: %f\n", parameters.tax_evaporate);
    printf("P exploitation: %f\n", parameters.p_exploitation);
}

/**
 * @brief Reset parameters in parameters_to_reset
 * Example: reset_parameters("p:d:s");
 * Isso vai resetar population_size, dimension e seed
 */

void reset_parameters(char *parameters_to_reset)
{
    char *tmp = strdup(parameters_to_reset);
    char *parameter = strtok(tmp, ":");
    while (parameter != NULL)
    {
        switch (parameter[0])
        {
        case 'f':
            parameters.function_number = 0;
            break;
        case 't':
            parameters.time_limit = 0;
            break;
        case 'i':
            parameters.island_size = 0;
            break;
        case 'p':
            parameters.population_size = 0;
            break;
        case 'd':
            parameters.dimension = 0;
            break;
        case 'l':
            parameters.domain_function.min = 0;
            break;
        case 'u':
            parameters.domain_function.max = 0;
            break;
        case 'g':
            parameters.num_generations_per_epoca = 0;
            break;
        case 'm':
            parameters.mutation_rate = 0;
            break;
        case 'c':
            parameters.crossover_rate = 0;
            break;
        case 'k':
            parameters.num_migrations = 0;
            break;
        case 'K':
            parameters.num_epocas = 0;
            break;
        case 'M':
            parameters.choice_random_migrate = 0.0;
            break;
        case 'F':
            parameters.F = 0.0;
            break;
        case 's':
            parameters.seed = 0;
            break;
        case 'v':
            parameters.c1 = 0.0;
            break;
        case 'z':
            parameters.c2 = 0.0;
            break;
        case 'C':
            parameters.clones = 0;
            break;
        case 'e':
            parameters.tax_evaporate = 0.0;
            break;
        case 'n':
            parameters.num_candidates = 0;
            break;
        case 'Q':
            parameters.num_pso = 0;
            break;
        case 'L':
            parameters.num_clonal = 0;
            break;
        case 'O':
            parameters.num_aco = 0;
            break;
        case 'G':
            parameters.num_genetico = 0;
            break;
        case 'D':
            parameters.num_diferencial = 0;
            break;
        case 'P':
            parameters.p_exploitation = 0.0;
            break;
        default:
            printf("Invalid option: %s\n", parameter);
            print_usage();
            exit(1);
            break;
        }
        parameter = strtok(NULL, ":");
    }
}