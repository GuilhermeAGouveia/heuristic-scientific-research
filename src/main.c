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

#define DEBUG(x) 

// void main(void) {
//     printf("Hello, World!\n");
// }

// int main(int argc, char *argv[])
// {

//     set_parameters(argc, argv); // Lê os parâmetros da linha de comando e repassa para as variáveis globais
//     // print_parameters();

//     populacao *result;
//     // Melhor semente até agora: 1676931005 (Funcao 3) - 301.356
//     // Melhor semente até agora: 1676935665 (Funcao 8) - 801.1393
//     switch (parameters.algorithm)
//     {
//     case PSO:
//         result = pso();
//         break;
//     case GA:
//         result = genetic();
//         break;
//     case DE:
//         result = diferencial();
//         break;
//     case ACO:
//         result = aco();
//         break;
//     case CLONALG:
//         result = clonalg();
//         break;
//     default:
//         printf("Invalid algorithm. Please use one of the following: p, g, d, a, c\n");
//         exit(1);
//     }

//     print_individuo(*get_best_of_population(*result), parameters.dimension, 0);
//     return 0;
// }


void migrate_(populacao *populations, int island_size, int dimension, domain domain_function)
{
    DEBUG(printf("\nmigrate_\n"););
    populacao *vizinho;
    // print_population(populations[0].individuos, populations[0].size, dimension, 1);

    for (int i = 0; i < island_size; i++)
    {
        DEBUG(printf("Populacao %d com %d individuos\n", i, populations[i].size););
        populacao current_island = populations[i];
        //print_population(current_island.individuos, current_island.size, dimension, 1);
        individuo *population = current_island.individuos;
        for (int k = 1; k <= parameters.num_migrations; k++)
        {
            individuo *melhor_individuo_da_populacao = &population[current_island.size - k];
            DEBUG(printf("Melhor individuo da populacao %d: %lf\n", i, melhor_individuo_da_populacao->fitness););
            for (int j = 0; j < 4; j++)
            {
                vizinho = current_island.neighbours[j];
                if (vizinho == NULL)
                    continue;
                individuo *neighbour_population = vizinho->individuos;
                individuo *pior_indivuduo_do_vizinho = get_worst_of_population(neighbour_population, vizinho->size);
                if (melhor_individuo_da_populacao->fitness > pior_indivuduo_do_vizinho->fitness)
                {
                    individuo *new_worst = generate_population(1, dimension, domain_function, parameters.function_number);
                    *pior_indivuduo_do_vizinho = *new_worst;
                }
                else
                    *pior_indivuduo_do_vizinho = *melhor_individuo_da_populacao;
            }
        }
    }
}

void set_neighbours(populacao *populations, int island_size)
{
    DEBUG(printf("\nset_neighbours\n"););
    for (int i = 0; i < island_size; i++)
    {
        DEBUG(printf("Populacao %d com %d individuos\n", i, populations[i].size););
        populacao* current_island = &populations[i];
        current_island->neighbours = calloc(4, sizeof(populacao *));
        current_island->neighbours[0] = &populations[(i + 1) % island_size];
        current_island->neighbours[1] = &populations[(i + 3) % island_size];
    }
}

void print_neighbours(populacao *populations, int island_size)
{
    DEBUG(printf("\nprint_neighbours\n"););
    for (int i = 0; i < island_size; i++)
    {
        printf("Populacao %d com %d individuos\n", i, populations[i].size);
        populacao current_island = populations[i];
        for (int j = 0; j < 4; j++)
        {
            populacao *vizinho = current_island.neighbours[j];
            if (vizinho == NULL)
                continue;
            printf("Populacao %d tem vizinho %d\n", i, vizinho->size);
        }
    }
}

void print_combinations(Array combinations, int nAlgorithms)
{
    for (int i = 0; i < combinations.size; i++)
    {
        printf("Combination %d: ", i);
        printVector(combinations.arr[i], nAlgorithms);
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    int nAlgorithms = 5;
    set_parameters(argc, argv); // Lê os parâmetros da linha de comando e repassa para as variáveis globais

    Array algorithmsConfigurations = generateComb(nAlgorithms);

    print_combinations(algorithmsConfigurations, nAlgorithms);

    int nConfigurations = algorithmsConfigurations.size;

    for (int conf = 0; conf < nConfigurations; conf++)
    {
        printf("Running %d of %d configurations\n", conf, nConfigurations);

        populacao *populations = malloc(nAlgorithms * sizeof(populacao));
        for (int i = 0; i < parameters.num_migrations; i++) 
        {

            for (int alg = 0; alg < nAlgorithms; alg++)
            {

                parameters.algorithm = (enum algorithm)(algorithmsConfigurations.arr[conf][nAlgorithms] + 1);
                printf("Running algorithm %s\n", translateIntToAlg(parameters.algorithm));
                populacao *result;
                // Melhor semente até agora: 1676931005 (Funcao 3) - 301.356
                // Melhor semente até agora: 1676935665 (Funcao 8) - 801.1393
                switch (parameters.algorithm)
                {
                case PSO:
                    populations[alg] = *pso();
                    break;
                case GA:
                    populations[alg] = *genetic();
                    break;
                case DE:
                    populations[alg] = *diferencial();
                    break;
                case ACO:
                    populations[alg] = *aco();
                    break;
                case CLONALG:
                    populations[alg] = *clonalg();
                    break;
                default:
                    printf("Invalid algorithm. Please use one of the following: p, g, d, a, c\n");
                    exit(1);
                }

                print_individuo(*get_best_of_population(populations[alg]), parameters.dimension, 0);
            }
            set_neighbours(populations, nAlgorithms);
            //print_neighbours(populations, nAlgorithms);
            printf("Migrating...\n");
            migrate(populations, nAlgorithms, 4, parameters.dimension, parameters.domain_function, parameters.function_number);
        }
    }
}