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

void set_neighbours(populacao **populations, int island_size)
{
    DEBUG(printf("\nset_neighbours\n"););
    for (int i = 0; i < island_size; i++)
    {
        DEBUG(printf("Populacao %d com %d individuos\n", i, populations[i]->size););
        populacao *current_island = populations[i];
        current_island->neighbours = calloc(4, sizeof(populacao *));
        current_island->neighbours[0] = populations[(i + 1) % island_size];
        current_island->neighbours[1] = populations[(i + 3) % island_size];
    }
}

void print_neighbours(populacao **populations, int island_size)
{
    DEBUG(printf("\nprint_neighbours\n"););
    for (int i = 0; i < island_size; i++)
    {
        printf("Populacao %d com %d individuos\n", i, populations[i]->size);
        populacao *current_island = populations[i];
        for (int j = 0; j < 4; j++)
        {
            populacao *vizinho = current_island->neighbours[j];
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
    set_parameters(argc, argv); // Lê os parâmetros da linha de comando e repassa para as variáveis globais

    individuo *gbest_individuo = NULL;
    individuo *pbest_individuo = NULL;
    populacao **populations = calloc(10, sizeof(populacao*));
    int *alg_set = convert_parameter_to_array(parameters.algorithms);
    printf("Algorithms: ");
    printVector(alg_set, parameters.num_algorithms);

    for (int i = 0; i < parameters.num_migrations; i++)
    {

        for (int alg_pos = 0; alg_pos < parameters.num_algorithms; alg_pos++)
        {
            enum algorithm alg = (enum algorithm) (alg_set[alg_pos]);

            // print alg array

            printf("Running algorithm %s\n", translateIntToAlg(alg));
            // Melhor semente até agora: 1676931005 (Funcao 3) - 301.356
            // Melhor semente até agora: 1676935665 (Funcao 8) - 801.1393
            switch (alg)
            {
            case PSO:
                populations[alg_pos] = pso(populations[alg_pos]);
                break;
            case GA:
                populations[alg_pos] = genetic(populations[alg_pos]);
                break;
            case DE:
                populations[alg_pos] = diferencial(populations[alg_pos]);
                break;
            case ACO:
                populations[alg_pos] = aco(populations[alg_pos]);
                break;
            case CLONALG:
                populations[alg_pos] = clonalg(populations[alg_pos]);
                break;
            default:
                printf("Invalid algorithm. Please use one of the following: p, g, d, a, c\n");
                exit(1);
            }

            pbest_individuo = get_best_of_population(*populations[alg_pos]);
            print_individuo(*pbest_individuo, parameters.dimension, alg_pos);
            if (gbest_individuo == NULL || pbest_individuo->fitness < gbest_individuo->fitness)
            {
                gbest_individuo = pbest_individuo;
            }
        }
        set_neighbours(populations, parameters.num_algorithms);
        //print_neighbours(populations, parameters.num_algorithms);
        printf("Migrating...\n");
        //migrate(populations, parameters.num_algorithms, 4, parameters.dimension, parameters.domain_function, parameters.function_number);
        random_random_migrate(populations, parameters.num_algorithms, 4, parameters.dimension, parameters.domain_function, parameters.function_number);


    }

    printf("Best %lf\n", gbest_individuo->fitness);

    for (int i = 0; i < parameters.num_algorithms; i++)
    {
        destroy_island(populations[i], 1);
    }

    return 0;
}
