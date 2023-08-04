#include <stdio.h>
#include <stdlib.h>
#include "algorithms/genetic.h"
#include "algorithms/pso.h"
#include "algorithms/clonalg.h"
#include "algorithms/aco.h"
#include "algorithms/diferencial.h"
#include "algorithms/parameters.h"
#include "algorithms/commom.h"
#include "libs/utils.h"

// void main(void) {
//     printf("Hello, World!\n");
// }

int main(int argc, char *argv[])
{

    set_parameters(argc, argv); // Lê os parâmetros da linha de comando e repassa para as variáveis globais
    // print_parameters();

    populacao *result;
    populacao *population = NULL;

    domain domainAux;
    domainAux.max= 100;
    domainAux.min = -100;
    population = generate_island(1, 5800, 10, domainAux, 5);
    //population = NULL;
    // Melhor semente até agora: 1676931005 (Funcao 3) - 301.356
    // Melhor semente até agora: 1676935665 (Funcao 8) - 801.1393
    switch (parameters.algorithm)
    {
    case PSO:
        //population = genetic();
        result = pso(population);
        break;
    case GA:
        result = genetic(population);
        break;
    case DE:
        result = diferencial(population);
        break;
    case ACO:
        result = aco(population);
        break;
    case CLONALG:
        result = clonalg(population);
        break;
    default:
        printf("Invalid algorithm. Please use one of the following: p, g, d, a, c\n");
        exit(1);
    }

    print_individuo(*get_best_of_population(*result), parameters.dimension, 0);
    return 0;
}
