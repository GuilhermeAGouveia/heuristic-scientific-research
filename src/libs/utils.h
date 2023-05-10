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

#ifndef UTILS_H
#include "types.h"
#define UTILS_H
#define DEBUG(x) 

void print_individuo(individuo individuo, int dimension, int id);
void print_population(individuo *pop, int tamanho_populacao, int dimension, int only_fitness);
double random_double(double min, double max);
void free_population(populacao *populations, int n_populations);

#endif /* UTILS_H */
