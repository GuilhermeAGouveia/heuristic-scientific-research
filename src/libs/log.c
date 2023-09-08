#include <stdlib.h>
#include <stdio.h>
#include "log.h"
#include "types.h"
#include "utils.h"
#define DEBUG(x)

static int first_time = 1;

void write_parameters(int epoca, int population, args parameters)
{
    DEBUG(printf("\nwrite_parameters\n"););
    char file_path[256];

    sprintf(file_path, "log/data/epoca_%d/population_%d/_parametros.dat", epoca, population);

    FILE *parameters_log_file = fopen(file_path, "w");
    fprintf(parameters_log_file, "Parameters:");
    fprintf(parameters_log_file, "\n algorithm: %s,\n function_number: %d,\n F: %lf,\n time_limit: %d,\n island_size: %d,\n population_size: %d,\n dimension: %d,\n domain function interval: [%lf, %lf],\n num_generations: %d,\n mutation_rate: %d,\n crossover_rate: %d,\n num_migrations: %d,\n num_epocas: %d\n", translateIntToAlg(parameters.current_algorithm), parameters.function_number, parameters.F, parameters.time_limit, parameters.island_size, parameters.population_size, parameters.dimension, parameters.domain_function.min, parameters.domain_function.max, parameters.num_generations_per_epoca, parameters.mutation_rate, parameters.crossover_rate, parameters.num_migrations, parameters.num_epocas + 1);
    DEBUG(printf("\nwrite_parameters: end\n"););
    fclose(parameters_log_file);
}

void clean_log_dir()
{
    DEBUG(printf("\nclean_log_dir\n"););

    system("rm -rf log/data/*");
}

int create_population_and_epoca_dir(int epoca_num, int population_num)
{
    DEBUG(printf("\ncreate_population_and_epoca_dir\n"););

    char cmd[256];
    sprintf(cmd, "mkdir -p log/data/epoca_%d/population_%d", epoca_num, population_num);
    return system(cmd);
}

void write_population_log(int epoca_num, int population_num, int generation_num, populacao populacao_atual, args parameters)
{
    DEBUG(printf("\nwrite_population_log\n"););
    char log_file_name[256];
    if (first_time)
    {
        clean_log_dir();
        first_time = 0;
    }

    create_population_and_epoca_dir(epoca_num, population_num);
    write_parameters(epoca_num, population_num, parameters);
    sprintf(log_file_name, "log/data/epoca_%d/population_%d/generation_%d.log", epoca_num, population_num, generation_num);
    FILE *log_file = fopen(log_file_name, "w");

    for (int i = 0; i < populacao_atual.size; i++)
    {
        FILE *log_file = fopen(log_file_name, "a");
        // fprintf(log_file, "Individuo %d: ", i);
        for (int j = 0; j < parameters.dimension; j++)
        {
            fprintf(log_file, "%f ", populacao_atual.individuos[i].chromosome[j]);
        }
        fprintf(log_file, "> %f\n", populacao_atual.individuos[i].fitness);
        fclose(log_file);
    }
}