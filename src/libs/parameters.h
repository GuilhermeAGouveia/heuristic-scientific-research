
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   lista.h
 * Author: Guilherme Gouveia
 *
 * Created on January 26, 2021, 11:32 AM
 */

typedef struct args_
{
    int function_number;
    domain domain_function;
    float choice_random_migrate;
    char algorithms[250];
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
    enum algorithm current_algorithm;
} args;

#ifndef PARAMETERS_H
#include "types.h"

#define PARAMETERS_H

extern args parameters;

void set_parameters(int argc, char *argv[]);
void print_parameters(args parameters);
void reset_parameters(char* parameters_to_reset);

#endif /* PARAMETERS_H */
