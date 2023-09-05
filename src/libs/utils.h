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

typedef struct array
{
    int **arr;
    int size;
} Array;

void print_individuo(individuo individuo, int dimension, int id);
void print_population(individuo *pop, int tamanho_populacao, int dimension, int only_fitness);
double random_double(double min, double max);
void free_population(populacao *populations, int n_populations);
int doubleEqual(double a, double b, int num_casas);
double calc_mean(individuo *population, int population_size);
double desvio_padrao(individuo *population, int population_size);
void copy_individuo(individuo *original, individuo *copia, int dimension);
Array generateComb(int n);
void printVector(int arr[], int n);
char *translateIntToAlg(enum algorithm alg);
int size_of_array(char *parameter);
int *convert_parameter_to_array(char *parameter);
int *get_algorithms(int *vet, int total);

#endif /* UTILS_H */
