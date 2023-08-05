#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#define NO_RECORDING
#include "../libs/funcoes_cec_2015/cec15_test_func.h"
#include "../libs/statistics.h"
#include "../libs/types.h"
#include "../libs/utils.h"
#include "../libs/crossover.h"
#include "../libs/log.h"
#define STATISTICS(x)
#define DEBUG(x) x
#define LOG(x)

static args parameters;

void print_usage()
{
    printf("Usage: ./evolucao_mpop -f <function_number> -t <time_limit> -i <island_size> -p <population_size> -d <dimension> -l <bounds_lower> -u <bounds_upper> -g <num_generations> -m <mutation_probability>");
}

void set_default_parameters_clonalg()
{
    parameters.F = 0.77294;
    parameters.function_number = 3;
    parameters.time_limit = 10; // seconds
    parameters.island_size = 1;
    parameters.population_size = 140;
    parameters.dimension = 10; // 10 or 30
    parameters.domain_function.min = -100;
    parameters.domain_function.max = 100;
    parameters.num_generations_per_epoca = 300;
    parameters.mutation_rate = 28;  // %
    parameters.crossover_rate = 89; // %
    parameters.num_migrations = 7;
    parameters.seed = time(NULL);
}

void set_parameters(int argc, char *argv[])
{
    int opt;
    set_default_parameters_clonalg();
    while ((opt = getopt(argc, argv, "f:F:t:i:p:d:l:u:g:m:c:k:s:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            parameters.function_number = atoi(optarg);
            break;
        case 't':
            parameters.time_limit = atoi(optarg);
            break;
        case 'i':
            parameters.island_size = atoi(optarg);
            break;
        case 'p':
            parameters.population_size = atoi(optarg);
            break;
        case 'd':
            parameters.dimension = atoi(optarg);
            break;
        case 'l':
            parameters.domain_function.min = atoi(optarg);
            break;
        case 'u':
            parameters.domain_function.max = atoi(optarg);
            break;
        case 'g':
            parameters.num_generations_per_epoca = atoi(optarg);
            break;
        case 'm':
            parameters.mutation_rate = atoi(optarg);
            break;
        case 'c':
            parameters.crossover_rate = atoi(optarg);
            break;
        case 'k':
            parameters.num_migrations = atoi(optarg);
            break;
        case 'F':
            parameters.F = atof(optarg);
            break;
        case 's':
            parameters.seed = atoi(optarg);
            break;
        default:
            printf("Invalid option: %c\n", opt);
            print_usage();
            exit(1);
            break;
        }
    }
}

void fitness(individuo *individuo, int dimension)
{
    // individuo.fitness = real_function(individuo.chromosome, dimension);
    cec15_test_func(individuo->chromosome, &individuo->fitness, dimension, 1, parameters.function_number);
    // double x = individuo->chromosome[0];
    // double y = individuo->chromosome[1];
    // individuo->fitness = pow(x, 2) + pow(y, 2) - cos(18 * x) - cos(18 * y);
    // printf("fitness: %f\n", individuo->fitness);
}

int comparador_individuo(const void *a, const void *b)
{
    individuo *v1 = (individuo *)a;
    individuo *v2 = (individuo *)b;

    return v1->fitness < v2->fitness;
}

void print_roleta(int *roleta, int roleta_size, int ball1, int ball2)
{
    DEBUG(printf("\nprint_roleta\n"););
    for (int i = 0; i < roleta_size; i++)
    {
        if (i == ball1 || i == ball2)
            printf("%d<-o ", roleta[i]);
        else
            printf("%d ", roleta[i]);
    }
    printf("\n\n");
}

int roleta_pais(individuo *populacao, int num_individuos)
{
    // DEBUG(printf("\nroleta\n"););
    int roulette[num_individuos + 100];
    double sum_beneficio = 0.0;
    double sum_beneficio_inv = 0.0;

    // DEBUG(printf("sum_beneficio[init]\n"););
    for (int i = 0; i < num_individuos; i++)
    {
        sum_beneficio += populacao[i].fitness;
    }

    for (int i = 0; i < num_individuos; i++)
    {
        sum_beneficio_inv += sum_beneficio - populacao[i].fitness;
    }
    // DEBUG(printf("sum_beneficio[end]: %lf\n", sum_beneficio););

    int base = 0;
    for (int i = 0; i < num_individuos; i++)
    {
        double individuo_beneficio_inv = sum_beneficio - populacao[i].fitness;
        // DEBUG(printf("individuo_beneficio: %lf\n", populacao[i].fitness););
        int limit = ceil((individuo_beneficio_inv / sum_beneficio_inv) * 100.00);
        // DEBUG(printf("Preenchendo roleta com %d de %d até %d\n", i, base, base + limit););
        for (int j = base; j < base + limit; j++)
        {
            roulette[j] = i;
        }
        base += limit;
    }
    // DEBUG(printf("Numero de elementos na rolêta: %d\n", base););
    // DEBUG(printf("Indice sorteado: %d\n", roulette[rand() % base]););
    return roulette[rand() % base];
}

void select_parents(populacao populacao, individuo *parents[2])
{
    DEBUG(printf("\nselect_parents\n"););
    int pai1 = roleta_pais(populacao.individuos, populacao.size), pai2;
    do
    {
        pai2 = roleta_pais(populacao.individuos, populacao.size);
    } while (pai1 == pai2);
    parents[0] = &populacao.individuos[pai1];
    parents[1] = &populacao.individuos[pai2];

    DEBUG(printf("Pais: %d e %d\n", pai1, pai2););
}

individuo *generate_population(int n_individuos, int dimension, domain domain_function)
{
    DEBUG(printf("\ngenerate_population\n"););
    individuo *population = malloc(n_individuos * sizeof(individuo));
    for (int i = 0; i < n_individuos; i++)
    {
        population[i].chromosome = (double *)malloc(dimension * sizeof(double));
        for (int j = 0; j < dimension; j++)
        {
            population[i].chromosome[j] = random_double(domain_function.min, domain_function.max);
        }
        fitness(&population[i], dimension);
    }
    return population;
}

populacao *generate_island(int island_size, int population_size, int dimension, domain domain_function)
{
    DEBUG(printf("\ngenerate_island\n"););
    populacao *populations = malloc(island_size * sizeof(populacao));
    populacao **neighbours = calloc(4, sizeof(populacao *));
    for (int i = 0; i < island_size; i++)
    {
        populations[i].individuos = generate_population(population_size, dimension, domain_function);
        populations[i].size = population_size;
        populations[i].crossover = rand() % 6;
        populations[i].neighbours = calloc(4, sizeof(populacao *));
        populations[i].neighbours[0] = &populations[(i + 1) % island_size]; // talvez isso dê problema
        populations[i].neighbours[1] = &populations[(i + 3) % island_size]; // talvez isso dê problema
    }
    return populations;
}

populacao *mutation_commom(populacao *populacao, int dimension, domain domain_function)
{
    DEBUG(printf("\nmutation\n"););
    for (int i = 0; i < populacao->size - 1; i++)
    {
        int mutation_point = rand() % dimension;
        populacao->individuos[i].chromosome[mutation_point] = random_double(domain_function.min, domain_function.max);
        fitness(&populacao->individuos[i], dimension);
    }
    return populacao;
}

populacao *mutation_diferencial(populacao *populacao_original, int dimension, domain domain_function)
{
    DEBUG(printf("\nMutation\n"););

    // TODO: populacao_mutada[i] = populacao_original não ocorre em NAO_MUTAR
    populacao *populacao_mutada = generate_island(1, populacao_original->size, dimension, domain_function);

    for (int i = 0; i < populacao_original->size; i++)
    {

        DEBUG(printf("\nIndividuo %d\n", i); print_individuo(populacao_original->individuos[i], dimension, i);)

        DEBUG(printf("individuo %d sofreu mutação\n", i););
        int alpha, beta, gamma;

        for (int j = 0; j < dimension; j++)
        {
            // double result = rand();
            double result = domain_function.max + 1;
            while (result > parameters.domain_function.max || result < domain_function.min)
            {
                do
                {
                    alpha = rand() % populacao_original->size;
                    beta = rand() % populacao_original->size;
                    gamma = rand() % populacao_original->size;
                } while (alpha == beta || alpha == gamma || beta == gamma);
                result = populacao_original->individuos[alpha].chromosome[j] + parameters.F * (fabs(populacao_original->individuos[beta].chromosome[j]) - fabs(populacao_original->individuos[gamma].chromosome[j]));
            }
            populacao_mutada->individuos[i].chromosome[j] = result;
        }
        fitness(&populacao_mutada->individuos[i], dimension);
        DEBUG(printf("Individuo %d\n", i); print_individuo(populacao_mutada->individuos[i], dimension, i);)
    }
    return populacao_mutada;
}

individuo *get_best_of_population(populacao populacao)
{
    DEBUG(printf("\nget_best_of_population\n"););
    // qsort(population, n_populacoes, sizeof(individuo), comparador_individuo);
    return &populacao.individuos[populacao.size - 1];
}

individuo *get_worst_of_population(individuo *population, int n_populacoes)
{
    qsort(population, n_populacoes, sizeof(individuo), comparador_individuo);
    return &population[0];
}

void selection(populacao *population_original, populacao *population_mutation, int dimension)
{
    DEBUG(printf("\nselection\n"););
    DEBUG(printf("População original\n"););
    DEBUG(print_population(population_original->individuos, population_original->size, dimension, 1););
    DEBUG(printf("População cruzamento\n"););
    DEBUG(print_population(population_mutation->individuos, population_mutation->size, dimension, 1););
    for (int i = 0; i < population_mutation->size; i++)
    {
        if (population_mutation->individuos[i].fitness < population_original->individuos[i].fitness)
            population_original->individuos[i] = population_mutation->individuos[i];
    }
    qsort(population_original->individuos, population_original->size, sizeof(individuo), comparador_individuo);
    DEBUG(printf("População selecionada\n"););
    DEBUG(print_population(population_original->individuos, population_original->size, dimension, 1););
}

/**
 * @brief Algoritmo Evolucionário
 * O algoritmo para quando:
 * 1. Cerca de 90% da população for avaliada como melhor do que o critério de seleção
 * 2. O tempo de execução for maior que 10 segundos
 * 3. O numero de gerações for 500
 * @param itens
 * @param n_itens
 * @param capacidad
 * @param population_size
 * @param select_criteria
 * @return int*
 */

individuo *get_pior_pai(individuo *pais[2])
{
    return pais[0]->fitness < pais[1]->fitness ? pais[1] : pais[0];
}

populacao *crossover(populacao *populacao_original, populacao *populacao_mutada, int dimension)
{
    populacao *nova_populacao = generate_island(1, populacao_original->size, dimension, parameters.domain_function);
    int sigma;
    for (int i = 0; i < populacao_original->size; i++)
    {
        sigma = rand() % parameters.dimension;
        for (int j = 0; j < dimension; j++)
        {
            if (rand() % 100 < parameters.mutation_rate || j == sigma)
                nova_populacao->individuos[i].chromosome[j] = populacao_mutada->individuos[i].chromosome[j];
            else
                nova_populacao->individuos[i].chromosome[j] = populacao_original->individuos[i].chromosome[j];
        }
        fitness(&nova_populacao->individuos[i], dimension);
    }
    return nova_populacao;
}

void migrate(populacao *populations, int island_size, int dimension, domain domain_function)
{
    DEBUG(printf("\nmigrate\n"););
    populacao *vizinho;
    // print_population(populations[0].individuos, populations[0].size, dimension, 1);

    for (int i = 0; i < island_size; i++)
    {
        DEBUG(printf("Populacao %d\n", i));
        populacao current_island = populations[i];
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
                individuo *pior_indivuduo_do_vizinho = get_worst_of_population(neighbour_population, parameters.population_size);
                if (melhor_individuo_da_populacao->fitness > pior_indivuduo_do_vizinho->fitness)
                {
                    individuo *new_worst = generate_population(1, dimension, domain_function);
                    *pior_indivuduo_do_vizinho = *new_worst;
                }
                else
                    *pior_indivuduo_do_vizinho = *melhor_individuo_da_populacao;
            }
        }
    }
}

void swap_individuo(individuo *a, individuo *b)
{
    individuo temp = *a;
    *a = *b;
    *b = temp;
}

void shuffle(individuo *pool, int size_pool)
{
    for (int i = 0; i < size_pool; i++)
        swap_individuo(&pool[rand() % size_pool], &pool[rand() % size_pool]);
}

void random_random_migrate(populacao *populations, int island_size, int dimension, domain domain_function)
{
    DEBUG(printf("\nrandom random migrate\n"););
    individuo *pool = generate_population(parameters.num_migrations * island_size, dimension, domain_function);
    int positions[island_size][parameters.num_migrations];
    for (int i = 0; i < island_size; i++)
    {
        DEBUG(printf("população %d\n", i););
        for (int j = 0; j < parameters.num_migrations; j++)
        {
            int rand_index = rand() % parameters.population_size;
            DEBUG(printf("posição sorteada %d\n", rand_index););
            if (!populations[i].individuos[rand_index].chromosome)
            { // se for nulo que dizer que o elemento já foi sorteado
                j--;
                DEBUG(printf("posição já foi sorteada\n"););
                continue;
            }

            pool[i * parameters.num_migrations + j] = populations[i].individuos[rand_index]; // corre o risco de sortear o mesmo individuo
            populations[i].individuos[rand_index] = (individuo){NULL, INFINITY, NULL};
            positions[i][j] = rand_index;
        }
    }

    shuffle(pool, parameters.num_migrations * island_size);

    for (int i = 0; i < island_size; i++)
    {
        for (int j = 0; j < parameters.num_migrations; j++)
        {
            populations[i].individuos[positions[i][j]] = pool[i * parameters.num_migrations + j];
        }
    }
}

individuo diferencial()
{
    DEBUG(printf("\nevolution\n"););
    individuo *parents[2];
    individuo bestIndividuo = {.fitness = INFINITY};
    populacao *populations = generate_island(parameters.island_size, parameters.population_size, parameters.dimension, parameters.domain_function);
    time_t time_init, time_now;
    int epoca_count = 0;
    time(&time_init);
    time(&time_now);
    int total_epocs_s_m = 0, continue_evol = 1, limit_epocs = 4;
    double best_ep_ant = bestIndividuo.fitness;
    DEBUG(printf("Iniciando evolucao\n"););

    // while (continue_evol)
    while (difftime(time_now, time_init) < parameters.time_limit && continue_evol)
    {
        printf("Epoca: %d\n", epoca_count);
        best_ep_ant = bestIndividuo.fitness;
        for (int i = 0; i < parameters.island_size; i++)
        {
            LOG(printf("\n\ni-ésima ilha: %d\n", i););
            populacao *original_population = &populations[i];
            populacao *cross_population;
            populacao *mutation_population;
            int generation_count = 0;
            int max_inter_add = 100;
            int max_inter = 200;
            int cont_or_stop = 1;
            double aux, best_anter = get_best_of_population(*original_population)->fitness;
            while (cont_or_stop && difftime(time_now, time_init) < parameters.time_limit)
            {
                while (generation_count < max_inter && difftime(time_now, time_init) < parameters.time_limit)
                {
                    time(&time_now);
                    mutation_population = mutation_diferencial(original_population, parameters.dimension, parameters.domain_function);
                    cross_population = crossover(original_population, mutation_population, parameters.dimension);
                    selection(original_population, cross_population, parameters.dimension);

                    // print_individuo(original_population->individuos[original_population->size - 1], parameters.dimension, 1);
                    LOG(write_population_log(epoca_count, i, generation_count, *original_population, parameters););
                    STATISTICS(print_coords(&original_population->individuos[original_population->size - 1], 1, generation_count, parameters.num_generations_per_epoca););
                    DEBUG(printf("\nGeração: %d\n", generation_count););
                    generation_count++;
                }
                aux = get_best_of_population(*original_population)->fitness;
                if (doubleEqual(best_anter, aux, 2))
                    cont_or_stop = 0;
                else
                {
                    best_anter = aux;
                    max_inter += max_inter_add;
                }
            }
            individuo *bestCurrent = get_best_of_population(*original_population);

            // puts("\nMelhor de toda a população:");
            // print_individuo(*bestCurrent, parameters.dimension);
            // printf("%lf\n", bestCurrent->fitness);
            if (bestCurrent->fitness < bestIndividuo.fitness)
                bestIndividuo = *bestCurrent;
        }
        migrate(populations, parameters.island_size, parameters.dimension, parameters.domain_function);
        epoca_count++;
        parameters.num_epocas = epoca_count;
        // Verifica se um best_Individuo foi encontrado em relaçao a epoca anterior
        if (doubleEqual(bestIndividuo.fitness, best_ep_ant, 4))
        {
            total_epocs_s_m++;
        }
        else
            total_epocs_s_m = 0;
        // Se o limite de epocas sem melhora for atingido é finalizada a evolucao
        if (total_epocs_s_m == limit_epocs)
            continue_evol = 0;
    }
    return bestIndividuo;
}

int main(int argc, char *argv[])
{
    set_parameters(argc, argv); // Lê os parâmetros da linha de comando e repassa para as variáveis globais
    // print_parameters();

    individuo result;
    // Melhor semente até agora: 1676931005 (Funcao 3) - 301.356
    // Melhor semente até agora: 1676935665 (Funcao 8) - 801.1393
    srand(parameters.seed);
    result = diferencial();

    print_individuo(result, parameters.dimension, 0);
    printf("Best %lf\n", result.fitness);
    return 0;
}
