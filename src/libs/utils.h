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

void print_individue(individue individuo, int dimension);
void print_population(individue *population, int n_populacoes, int dimension);
double random_double(double min, double max);

#endif /* UTILS_H */
