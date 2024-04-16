/**
 * @brief Este algoritmo de ilha aceita que seja passado qual o conjunto de algoritmos que irá compor o conjunto de ilhas. Os parâmetros são:
 * -A: conjunto de algoritmos que irão compor o conjunto de ilhas.
 * Exemplo: -A 1,2,3,4,2:
 * Cada valor representa a quantidade de ilhas para cada algoritmo
 * Ordem:
 * 0 - PSO
 * 1 - DE
 * 2 - ACO
 * 3 - CLONALG
 * 4 - GA
 *
 * -K indica o intervalo entre migrações
 * -k a quantidade de indivíduos a serem migrados
 * Exemplo de execução: ./evol -A 1,2,3,4,2 -K 2 -k 3
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

int main(int argc, char *argv[])
{
    set_parameters(argc, argv); // Lê os parâmetros da linha de comando e repassa para as variáveis globais
    individuo *gbest_individuo = generate_population(1, 10, parameters.domain_function, 15);
    individuo *pbest_individuo = generate_population(1, 10, parameters.domain_function, 15);
    gbest_individuo->fitness = INFINITY;
    time_t time_init, time_now;
    populacao **populations = calloc(parameters.num_algorithms, sizeof(populacao *));
    double convergence_current = INFINITY;
    int current_generation = 0, current_gen_alg = 0, generations_to_calcDensity = 100, make_migration, calculate_density;
    int limit_time = 3600, convergence_expected = 50, final_attempts = 10, attempt_control, epoca = 0;
    int *alg_set = convert_parameter_to_array(parameters.algorithms);
    alg_set = get_algorithms(alg_set, parameters.num_algorithms);
    parameters.num_generations_per_epoca = minimum(parameters.num_epocas, generations_to_calcDensity);
    make_migration = parameters.num_epocas;
    attempt_control = final_attempts;
    calculate_density = generations_to_calcDensity;
    printf("Algorithms: ");
    printVector(alg_set, parameters.num_algorithms);
    time(&time_init);
    time(&time_now);
    while (difftime(time_now, time_init) < limit_time && (convergence_current > convergence_expected))
    {
        // printf("\nconverged:%i, epoca: %i, aux: %i\n", converged, epoca, aux);

        for (int alg_pos = 0; alg_pos < parameters.num_algorithms; alg_pos++)
        {
            enum algorithm alg = (enum algorithm)(alg_set[alg_pos]);

            // printf("Running algorithm %s\n", translateIntToAlg(alg));

            switch (alg)
            {
            case PSO:
                populations[alg_pos] = pso(populations[alg_pos], epoca, current_gen_alg, alg_pos);
                break;
            case GA:
                populations[alg_pos] = genetic(populations[alg_pos], epoca, current_gen_alg, alg_pos);
                break;
            case DE:
                populations[alg_pos] = diferencial(populations[alg_pos], epoca, current_gen_alg, alg_pos);
                break;
            case ACO:
                populations[alg_pos] = aco(populations[alg_pos], epoca, current_gen_alg, alg_pos);
                break;
            case CLONALG:
                populations[alg_pos] = clonalg(populations[alg_pos], epoca, current_gen_alg, alg_pos);
                break;
            default:
                printf("Invalid algorithm. Please use one of the following: p, g, d, a, c\n");
                exit(1);
            }
            copy_individuo(get_best_of_population(*populations[alg_pos]), pbest_individuo, parameters.dimension);
            // print_individuo(*pbest_individuo, parameters.dimension, alg_pos);
            if (pbest_individuo->fitness < gbest_individuo->fitness)
            {
                copy_individuo(pbest_individuo, gbest_individuo, parameters.dimension);
            }
        }
        set_neighbours(populations, parameters.num_algorithms);
        // print_neighbours(populations, parameters.num_algorithms);
        current_generation += parameters.num_generations_per_epoca;
        current_gen_alg += parameters.num_generations_per_epoca;

        // Abaixo verificamos o que será feito: uma migração ou um calculo de convergência. Caso a convergência esperada seja alcançada o programa continuara em execução até que se esgote o número máximo de tentativas finais de melhora do resultado, e caso ocorra a melhora o número de tentativas finais é resetado
        if (current_generation == make_migration)
        {

            if (((float)rand() / RAND_MAX) < parameters.choice_random_migrate)
            {
                printf("Migrating [RANDOM]...\n");
                random_random_migrate(populations, parameters.num_algorithms, parameters.num_migrations, parameters.dimension, parameters.domain_function, parameters.function_number);
            }
            else
            {
                printf("Migrating [BEST]...\n");
                migrate(populations, parameters.num_algorithms, parameters.num_migrations, parameters.dimension, parameters.domain_function, parameters.function_number);
            }
            make_migration += parameters.num_epocas;
        }

        if (current_generation == calculate_density)
        {
            printf("\nCalculate Convergense...\n");
            convergence_current = convergence_calculation_islands(populations, parameters.num_algorithms);
            if (convergence_current <= convergence_expected && attempt_control)
            {
                convergence_current = INFINITY;
                attempt_control--;
            }
            else
            {
                attempt_control = final_attempts;
            }
            calculate_density += generations_to_calcDensity;
            epoca++;
            current_gen_alg = 0;
            printf("Current_generation: %d,  make_migration: %d, calculate_density: %d Convergence Current:%lf\n", current_generation, make_migration, calculate_density, convergence_current);
            time(&time_now);
        }
        parameters.num_generations_per_epoca = minimum(make_migration, calculate_density) - current_generation;
    }


    printf("Best %lf\n", gbest_individuo->fitness);

    for (int i = 0; i < parameters.num_algorithms; i++)
    {
        destroy_island(populations[i], 1);
    }

    return 0;
}