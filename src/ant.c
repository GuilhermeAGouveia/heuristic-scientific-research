#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 100 // Número máximo de nós no grafo
#define M 50 // Número máximo de formigas
#define ALPHA 1 // Influência da trilha de feromônio na escolha do próximo nó
#define BETA 2 // Influência da heurística na escolha do próximo nó
#define RHO 0.5 // Taxa de evap
