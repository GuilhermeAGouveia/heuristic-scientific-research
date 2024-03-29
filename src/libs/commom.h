
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
#ifndef COMMOM_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#include "types.h"
#include "funcoes_cec_2015/cec15_test_func.h"
#include "funcoes_cec_2015/cec15_test_func.h"
#include "statistics.h"
#include "types.h"
#include "utils.h"
#include "crossover.h"
#include "log.h"


#define NO_RECORDING
#define STATISTICS(x)
#define DEBUG(x) x
#define LOG(x) 


#define COMMOM_H

void fitness(individuo *individuo, int dimension, int function_number);
int comparador_individuo(const void *a, const void *b);
void copy_individuo(individuo *original, individuo *copia, int dimension);
individuo *generate_population(int n_individuos, int dimension, domain domain_function, int function_number);
populacao *generate_clean_island(int island_size, int population_size, int dimension);
individuo *generate_clean_population(int n_individuos, int dimension);
void destroy_population(individuo *population, int n_individuos);
populacao *generate_island(int island_size, int population_size, int dimension, domain domain_function, int function_number);
void destroy_island(populacao *populations, int island_size);
individuo *get_best_of_population(populacao populacao);
individuo *get_worst_of_population(individuo *population, int n_populacoes);
int comparador_proporcoes_alg(const void *a, const void *b);
void migrate(populacao **populations, int island_size, int num_migrations, int dimension, domain domain_function, int function_number);
void random_random_migrate(populacao **populations, int island_size, int num_migrations, int dimension, domain domain_function, int function_number);

#endif /* COMMOM_H */
