
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
#ifndef PARAMETERS_H
#include "../libs/types.h"
#include "../libs/log.h"

#define PARAMETERS_H

extern args parameters;

void set_parameters(int argc, char *argv[]);
void print_parameters(args parameters);

#endif /* PARAMETERS_H */
