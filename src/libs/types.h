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

typedef struct individuo_
{
    double *chromosome;
    double fitness;
    float inter_Pai_inicio;
    float inter_Pai_fim;
} individuo;


typedef struct population_
{
    individuo *individuos;
    int size;
    struct population_ **neighbours;
    int crossover;

} populacao;

typedef struct domain
{
    double min;
    double max;
} domain;

#endif /* TYPES_H */
