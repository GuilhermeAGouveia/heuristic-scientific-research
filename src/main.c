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
#include <math.h>

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

void get_algoritms(int *result){
    double total = parameters.num_aco + parameters.num_ants + parameters.num_clonal + parameters.num_diferencial + parameters.num_genetico;
    int aux, totais[5];

    int cont = 0;

    totais[0] = ceil(parameters.num_algorithms * (parameters.num_pso / total));
    totais[1] =  ceil(parameters.num_algorithms * (parameters.num_diferencial / total));
    totais[2] = ceil(parameters.num_algorithms * (parameters.num_aco / total));
    totais[3] = ceil(parameters.num_algorithms * (parameters.num_clonal / total));
    totais[4] = ceil(parameters.num_algorithms * (parameters.num_genetico / total));
    total = 0;

   for(int i = 0; i < 5; i++)
     total += totais[i];
    
  //  while(total > parameters.num_algorithms){
  //      aux = rand() % 5;
   //     totais[aux] -= 1;
   //     total -= 1;
  //  }

    while(total > parameters.num_algorithms){
        for(int i = 0; total > parameters.num_algorithms && i < 5; i++){
            totais[i] -= 1;
            total -= 1;
        }
    }

    for(int i = 0; i < 5; i++){
        for(int j = 0; j < totais[i]; j++, cont++){
            result[cont] = i;
        }
    }
    printVector(result, parameters.num_algorithms);

}

int main(int argc, char *argv[])
{
    // ./evol  -t 1 -K 2 -k 3 -Q 30 -G 0 -D 68 -O 0 -L 49 -A 10
    set_parameters(argc, argv); // Lê os parâmetros da linha de comando e repassa para as variáveis globais
    int *algoritmos = calloc(parameters.num_algorithms, sizeof(int *));;
    get_algoritms(algoritmos);
    individuo *gbest_individuo = NULL;
    individuo *pbest_individuo = NULL;
    populacao **populations = calloc(10, sizeof(populacao *));
    //int *alg_set = convert_parameter_to_array(parameters.algorithms);
    //printf("Algorithms: ");
    //printVector(alg_set, parameters.num_algorithms);

    for (int i = 0; i < parameters.num_epocas; i++)
    {

        for (int alg_pos = 0; alg_pos < parameters.num_algorithms; alg_pos++)
        {
            enum algorithm alg = (enum algorithm)(algoritmos[alg_pos]);
            parameters.seed = time(NULL);

            printf("Running algorithm %s\n", translateIntToAlg(alg));

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
        // print_neighbours(populations, parameters.num_algorithms);
        printf("Migrating...\n");
        if (((float)rand() / RAND_MAX) < parameters.choice_random_migrate)
            random_random_migrate(populations, parameters.num_algorithms, parameters.num_migrations, parameters.dimension, parameters.domain_function, parameters.function_number);
        else
            migrate(populations, parameters.num_algorithms, parameters.num_migrations, parameters.dimension, parameters.domain_function, parameters.function_number);
    }

    printf("Best %lf\n", gbest_individuo->fitness);

    for (int i = 0; i < parameters.num_algorithms; i++)
    {
        destroy_island(populations[i], 1);
    }

    return 0;
}
