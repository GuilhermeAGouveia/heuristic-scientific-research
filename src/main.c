#include <stdio.h>
#include <stdlib.h>
#include "algorithms/genetic.h"
#include "algorithms/pso.h"
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
    // Melhor semente até agora: 1676931005 (Funcao 3) - 301.356
    // Melhor semente até agora: 1676935665 (Funcao 8) - 801.1393
    switch (parameters.algorithm)
    {
    case PSO:
        result = pso();
        break;
    case GA:
        result = genetic();
        break;
    case DE:
        // result = de();
        printf("DE not implemented yet\n");
        break;
    case ANT:
        // result = ant();
        printf("ANT not implemented yet\n");
        break;
    case CLONALG:
        // result = clonalg();
        printf("CLONALG not implemented yet\n");
        break;
    default:
        printf("Invalid algorithm. Please use one of the following: p, g, d, a, c\n");
        exit(1);
    }

    print_individuo(*get_best_of_population(*result), parameters.dimension, 0);
    return 0;
}
