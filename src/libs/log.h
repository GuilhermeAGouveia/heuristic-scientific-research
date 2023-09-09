#ifndef LOG_H
#include "types.h"
#include "parameters.h"
#define LOG_H

int create_population_and_epoca_dir(int, int);
void write_population_log(int epoca_num, int population_num, int generation_num, populacao populacao_atual, args parameters);

#endif /* LOG_H */
