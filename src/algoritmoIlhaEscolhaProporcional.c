/**
 * @brief Este algoritmo de ilha seleciona os algoritmos que irão compor o conjunto de ilha com base na proporção de cada um, que é passada por parâmetro. Os parâmetros são:
 * -Q: proporção do PSO
 * -G: proporção do GA
 * -D: proporção do DE
 * -O: proporção do ACO
 * -L: proporção do CLONALG
 * O tamanho do conjunto de ilhas é passado por parâmetro com a flag -N.
 * 
 * Exemplo de execução: ./evol -N 10 -Q 30 -G 0 -D 68 -O 0 -L 49 -K 2
 * 
 * @author @gustavo1902
 * @author @ViniciusBastoss
 * @author @caioreius
 * @author @GuilhermeAGouveia
 * Orientador: Prof. Dr. @iagoac
 * @date 2023-08-31
*/


#include "algorithms/genetic.h"
#include "algorithms/pso.h"
#include "algorithms/clonalg.h"
#include "algorithms/aco.h"
#include "algorithms/diferencial.h"
#include "libs/commom.h"



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

void return_evol(int *vet , int size){
    int totais[5];
    totais[0] = 0;
    totais[1] = 0;
    totais[2] = 0;
    totais[3] = 0;
    totais[4] = 0;
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < size; j++){
            while (vet[j] != i && j+1 < size){
                j++;
            }
            if(vet[j] == i)
               totais[i] += 1;
        }

    }
    //printf("./evol -K %d -k %d -M %lf -A %d,%d,%d,%d,%d\n", parameters.num_epocas, parameters.num_migrations, parameters.choice_random_migrate, totais[0], totais[1], totais[2], totais[3], totais[4]);
    printf("%d,%d,%d,%lf,%d,%d,%d,%d,%d\n",parameters.num_algorithms, parameters.num_epocas, parameters.num_migrations, parameters.choice_random_migrate, totais[0], totais[1], totais[2], totais[3], totais[4]);
}
void get_algoritms(int *result){

    double soma_proporcoes = parameters.num_pso + parameters.num_aco + parameters.num_clonal + parameters.num_diferencial + parameters.num_genetico;
    int cont = 0, total_islands = 0;
    double total_islands_double;
    proporcao_alg proporcoes[5];

    proporcoes[0].proporcao = parameters.num_pso;
    proporcoes[1].proporcao = parameters.num_diferencial;
    proporcoes[2].proporcao = parameters.num_aco;
    proporcoes[3].proporcao = parameters.num_clonal;
    proporcoes[4].proporcao = parameters.num_genetico;

    for(int i = 0; i < 5; i++){
        total_islands_double = parameters.num_algorithms * (proporcoes[i].proporcao/soma_proporcoes);
        proporcoes[i].alg = i;
        if((total_islands_double - (int)total_islands_double) < 0.5)
          proporcoes[i].total_islands = (int)total_islands_double;
        else
           proporcoes[i].total_islands = 1 + (int)total_islands_double;
        total_islands += proporcoes[i].total_islands;
    }

    qsort(&proporcoes, 5, sizeof(proporcao_alg), comparador_proporcoes_alg);

    //for(int i = 0; i < 5; i++){
      //  printf("proporção:%d\n",proporcoes[i].proporcao);
   // }

   //adiciona ilhas aos algoritmos até que total == num_algoritmos
    while(total_islands < parameters.num_algorithms){
        for(int i = 0; total_islands < parameters.num_algorithms && i < 5; i++){
            proporcoes[i].total_islands += 1;
            total_islands += 1;
        }
    }

    while(total_islands > parameters.num_algorithms){
        for(int i = 5; total_islands > parameters.num_algorithms && i > 0; i--){
            proporcoes[i].total_islands -= 1;
            total_islands -= 1;
        }
    }
    
    //cria o vetor de algoritmos [0,0,0,1,1...]
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < proporcoes[i].total_islands; j++, cont++){
            result[cont] = proporcoes[i].alg;
        }
    }
    printVector(result, parameters.num_algorithms);
    //return_evol(result, parameters.num_algorithms);

}

void print_arr_int(int *arr, int size){
    for(int i = 0; i < size; i++){
        if (i == size - 1)
            printf("%d", arr[i]);
        else
        printf("%d,", arr[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    // ./evol  -t 1 -K 2 -k 3 -Q 30 -G 0 -D 68 -O 0 -L 49 -A 10
    set_parameters(argc, argv); // Lê os parâmetros da linha de comando e repassa para as variáveis globais
    int *algoritmos = calloc(parameters.num_algorithms, sizeof(int *));;
    get_algoritms(algoritmos);
    
    individuo *gbest_individuo = generate_population(1, 10, parameters.domain_function, 15);
    individuo *pbest_individuo = generate_population(1, 10, parameters.domain_function, 15);
    gbest_individuo->fitness = -10;
    populacao **populations = calloc(parameters.num_algorithms, sizeof(populacao *));
    //int *alg_set = convert_parameter_to_array(parameters.algorithms);
    //printf("Algorithms: ");
    //printVector(alg_set, parameters.num_algorithms);

    for (int epoca = 0; epoca < parameters.num_epocas; epoca++)
    {

        for (int alg_pos = 0; alg_pos < parameters.num_algorithms; alg_pos++)
        {
  
            enum algorithm alg = (enum algorithm)(algoritmos[alg_pos]);

            printf("Running algorithm %s\n", translateIntToAlg(alg));

            switch (alg)
            {
            case PSO:
                populations[alg_pos] = pso(populations[alg_pos], epoca, alg_pos);
                break;
            case GA:
                populations[alg_pos] = genetic(populations[alg_pos], epoca, alg_pos);
                break;
            case DE:
                populations[alg_pos] = diferencial(populations[alg_pos], epoca, alg_pos);
                break;
            case ACO:
                populations[alg_pos] = aco(populations[alg_pos], epoca, alg_pos);
                break;
            case CLONALG:
                populations[alg_pos] = clonalg(populations[alg_pos], epoca, alg_pos);
                break;
            default:
                printf("Invalid algorithm. Please use one of the following: p, g, d, a, c\n");
                exit(1);
            }
            //print_population(populations[alg_pos]->individuos, 10, 10, 0);

            //pbest_individuo = get_best_of_population(*populations[alg_pos]);
            copy_individuo(get_best_of_population(*populations[alg_pos]),pbest_individuo, parameters.dimension);
            print_individuo(*pbest_individuo, parameters.dimension, alg_pos);
            
            if (gbest_individuo->fitness < 0 || pbest_individuo->fitness < gbest_individuo->fitness)
            {
                //gbest_individuo = pbest_individuo;
                copy_individuo(pbest_individuo,gbest_individuo, parameters.dimension);
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
