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

#ifndef TYPES_H
#define TYPES_H
#include <math.h>


enum algorithm
{
    PSO = 0,
    DE,
    ACO,
    CLONALG,
    GA,
};
typedef struct individuo_
{
    double *chromosome;
    double fitness;
    double *velocidade;
} individuo;

typedef struct population_
{
    individuo *individuos;
    int size;
    struct population_ **neighbours;
    int crossover;
    int n_mutations;
    enum algorithm algorithm;

} populacao;

typedef struct proporcao_alg_
{
    int alg;
    int proporcao;
    int total_islands;
}proporcao_alg;

typedef struct domain
{
    double min;
    double max;
} domain;

#endif /* TYPES_H */
