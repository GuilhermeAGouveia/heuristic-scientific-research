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
    print_arr_int(algoritmos, parameters.num_algorithms);
    
    return 0;
}