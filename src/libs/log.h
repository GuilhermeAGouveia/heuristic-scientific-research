#ifndef LOG_H
#include "types.h"
#define LOG_H


typedef struct args_
{
    int function_number;
    domain domain_function;
    float choice_random_migrate;
    char algorithms[100];
    int num_algorithms;
    double F;
    int num_pso;
    int num_aco;
    int num_genetico;
    int num_clonal;
    int num_diferencial;
    int time_limit;
    int island_size;
    int population_size;
    int dimension;
    int crossover_rate;
    int mutation_rate;
    int num_migrations;
    int num_generations_per_epoca;
    int num_epocas;
    int evaluation_limit;
    int seed;
    double c1;
    double c2;
    int clones;
    double tax_evaporate;
    double num_candidates;
    double p_exploitation;
    int num_ants;

} args;

int create_population_and_epoca_dir(int, int);
void write_population_log(int epoca_num, int population_num, int generation_num, populacao populacao_atual, args parameters);

#endif /* LOG_H */
