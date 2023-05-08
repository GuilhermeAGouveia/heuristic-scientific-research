#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "statistics.h"
#define STATISTICS_DIR "../statistics"


int print_coords(individuo *population, int n_populacoes, int generation, int generation_max)
{   
    //printf("Inserindo dados de estatistica da população em %s\n", STATISTICS_DIR);
    if (generation_max > 30 && generation % (generation_max / 30) != 0)
    {
        return 0;
    }
    
    FILE *fp;
    char path[100] = STATISTICS_DIR;
    strcat(path, "/coords.dat");
    //printf("Inserindo dados de estatistica da população em %s\n", path);
    fp = fopen(path, "a");
    if (fp == NULL)
    {
        printf("Erro ao abrir arquivo de estatisticas\n");
        return 1;
    }
    for (int i = 0; i < n_populacoes; i++)
    {
        fprintf(fp, "%d %lf\n", generation, population[i].fitness);
    }
    return 0;
}