
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
#ifndef ACO_H
#include "../libs/types.h"

#define ACO_H

double abs_double(double x);
void initialize(individuo *individuos, int n, int d);
void update_pheromones(double **pheromones, individuo *individuos, int n, int d, individuo *best_individuo);
void select_next_position(double **pheromones, individuo *individuos, int d, individuo *best_individuo);
populacao *aco(populacao *population, int epoca_num,int current_generation, int population_num);

#endif /* ACO_H */
