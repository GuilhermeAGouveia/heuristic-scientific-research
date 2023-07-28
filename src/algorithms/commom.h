
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
#include "../libs/types.h"

#define COMMOM_H


void fitness(individuo *individuo, int dimension, int function_number);
int comparador_individuo(const void *a, const void *b);
individuo *generate_population(int n_individuos, int dimension, domain domain_function, int function_number);
populacao *generate_island(int island_size, int population_size, int dimension, domain domain_function, int function_number);
individuo *get_best_of_population(populacao populacao);
individuo *get_worst_of_population(individuo *population, int n_populacoes);
void clone_individue(individuo *clone, individuo *original, int dimension);

#endif /* COMMOM_H */
