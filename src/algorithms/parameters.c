#include <getopt.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../libs/utils.h"
#include "../libs/log.h"
#include "./parameters.h"

void print_usage()
{
    printf("Usage: ./evolucao_mpop -f <function_number> -t <time_limit> -i <island_size> -p <population_size> -d <dimension> -l <bounds_lower> -u <bounds_upper> -g <num_generations> -m <mutation_probability>");
}

args parameters;

void set_parameters(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "A:f:F:t:i:p:d:l:u:g:m:c:k:s:v:z:C:p:e:a:K:")) != -1)
    {
        switch (opt)
        {
        case 'A':

            parameters.num_algorithms = size_of_array(optarg);
            strcpy(parameters.algorithms, optarg);
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
        case 'F':
            parameters.F = atof(optarg);
            break;
        case 's':
            parameters.seed = atoi(optarg);
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
        case 'a':
            parameters.num_ants = atoi(optarg);
            break;
        case 'e':
            parameters.tax_evaporate = atof(optarg);
            break;
        case 'P':
            parameters.p_exploitation = atof(optarg);
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
    printf("Number of ants: %d\n", parameters.num_ants);
    printf("Tax evaporate: %f\n", parameters.tax_evaporate);
    printf("P exploitation: %f\n", parameters.p_exploitation);
}