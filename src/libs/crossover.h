typedef enum
{
    MEDIA,
    MEDIA_GEOMETRICA,
    METADE,
    PONTO,
    FLAT,
    BLEND,
} crossover_type;

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
#ifndef CROSSOVER_H
#include "types.h"
#define CROSSOVER_H


void cruzamento_media(individuo parent1, individuo parent2, int n_itens, individuo *child);
void cruzamento_flat(individuo parent1, individuo parent2, int n_itens, individuo *child);
void cruzamento_blend(individuo parent1, individuo parent2, int n_itens, individuo *child);
void cruzamento_metade(individuo parent1, individuo parent2, int n_itens, individuo *child);
void cruzamento_ponto(individuo parent1, individuo parent2, int n_itens, individuo *child);

#endif /* CROSSOVER_H */
