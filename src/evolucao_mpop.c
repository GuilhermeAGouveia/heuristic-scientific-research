#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#define NO_RECORDING
#include "./libs/funcoes_cec_2015/cec15_test_func.h"
#include "./libs/statistics.h"
#include "./libs/types.h"
#include "./libs/utils.h"
#include "./libs/crossover.h"
#define STATISTICS(x) 
#define DEBUG(x) 

static double F = 0.99;
static int function_number = 1;
static int time_limit = 10; // seconds
static int island_size = 10;
static int population_size = 3;
static int dimension = 10; // 10 or 30
static int bounds_lower = -100;
static int bounds_upper = 100;
static int num_generations = 300;
static int mutation_probability = 100; // %

void print_usage()
{
    printf("Usage: ./evolucao_mpop -f <function_number> -t <time_limit> -i <island_size> -p <population_size> -d <dimension> -l <bounds_lower> -u <bounds_upper> -g <num_generations> -m <mutation_probability>");
}

void set_parameters(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "f:F:t:i:p:d:l:u:g:m:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            function_number = atoi(optarg);
            break;
        case 't':
            time_limit = atoi(optarg);
            break;
        case 'i':
            island_size = atoi(optarg);
            break;
        case 'p':
            population_size = atoi(optarg);
            break;
        case 'd':
            dimension = atoi(optarg);
            break;
        case 'l':
            bounds_lower = atoi(optarg);
            break;
        case 'u':
            bounds_upper = atoi(optarg);
            break;
        case 'g':
            num_generations = atoi(optarg);
            break;
        case 'm':
            mutation_probability = atoi(optarg);
            break;
        case 'F':
            F = atof(optarg);
            break;
        default:
            printf("Invalid option: %c\n", opt);
            print_usage();
            exit(1);
            break;
        }
    }
}

void print_parameters()
{
    puts("Parameters:");
    printf(" function_number: %d,\n F: %f,\n time_limit: %d,\n island_size: %d,\n population_size: %d,\n dimension: %d,\n bounds_lower: %d,\n bounds_upper: %d,\n num_generations: %d,\n mutation_probability: %d\n", function_number, F, time_limit, island_size, population_size, dimension, bounds_lower, bounds_upper, num_generations, mutation_probability);
}

void fitness(individuo *individuo, int dimension)
{
    // individuo.fitness = real_function(individuo.chromosome, dimension);
    cec15_test_func(individuo->chromosome, &individuo->fitness, dimension, 1, function_number);
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

int roleta_pais(individuo *populacao, int n_populacoes)
{
    // DEBUG(printf("\nroleta\n"););
    int roulette[100];
    double sum_beneficio = 0.0;
    double sum_beneficio_inv = 0.0;

    // DEBUG(printf("sum_beneficio[init]\n"););
    for (int i = 0; i < n_populacoes; i++)
    {
        sum_beneficio += populacao[i].fitness;
    }

    for (int i = 0; i < n_populacoes; i++)
    {
        sum_beneficio_inv += sum_beneficio - populacao[i].fitness;
    }
    // DEBUG(printf("sum_beneficio[end]: %lf\n", sum_beneficio););

    int base = 0;
    for (int i = 0; i < n_populacoes; i++)
    {
        double individuo_beneficio_inv = sum_beneficio - populacao[i].fitness;
        // DEBUG(printf("individuo_beneficio: %lf\n", populacao[i].fitness););
        int limit = floor((individuo_beneficio_inv / sum_beneficio_inv) * 100.00);
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

void mutation_commom(populacao *populacao, int dimension, domain domain_function)
{
    DEBUG(printf("\nmutation\n"););
    for (int i = 0; i < populacao->size - 1; i++)
    {
        int mutation_point = rand() % dimension;
        populacao->individuos[i].chromosome[mutation_point] = random_double(domain_function.min, domain_function.max);
        fitness(&populacao->individuos[i], dimension);
    }
}

populacao mutation_diferencial(populacao *populacao, int dimension, domain domain_function)
{
    DEBUG(printf("\nMutation\n"););

    for (int i = 0; i < populacao->size - 1; i++)
    {

        DEBUG(printf("\nIndividuo %d\n", i); print_individuo(populacao->individuos[i], dimension);)
        if (rand() % 100 > mutation_probability)
        {
            DEBUG(printf("individuo %d não sofreu mutação\n", i););
            continue;
        }

        DEBUG(printf("individuo %d sofreu mutação\n", i););
        int alpha, beta, gamma;

        do
        {
            alpha = rand() % populacao->size;
            beta = rand() % populacao->size;
            gamma = rand() % populacao->size;
        } while (alpha == beta || alpha == gamma || beta == gamma);

        for (int j = 0; j < dimension; j++)
        {
            // double result = rand();
            double result = populacao->individuos[alpha].chromosome[j] + F * (fabs(populacao->individuos[beta].chromosome[j]) - fabs(populacao->individuos[gamma].chromosome[j]));
            populacao->individuos[i].chromosome[j] = result;
        }
        fitness(&populacao->individuos[i], dimension);
        DEBUG(printf("Individuo %d\n", i); print_individuo(populacao->individuos[i], dimension);)
    }
    return *populacao;
}

individuo *get_best_of_population(populacao populacao)
{
    DEBUG(printf("\nget_best_of_population\n"););
    // qsort(population, n_populacoes, sizeof(individuo), comparador_individuo);
    return &populacao.individuos[populacao.size - 1];
}

individuo *get_worst_of_population(individuo *population, int n_populacoes)
{
    // qsort(population, n_populacoes, sizeof(individuo), comparador_individuo);
    return &population[0];
}

populacao selection(populacao *population_original, populacao *population_crossover, int dimension)
{
    DEBUG(printf("\nselection\n"););
    DEBUG(printf("População original\n"););
    DEBUG(print_population(population_original->individuos, population_original->size, dimension, 1););
    DEBUG(printf("População cruzamento\n"););
    DEBUG(print_population(population_crossover->individuos, population_crossover->size, dimension, 1););
    for (int i = 0; i < population_crossover->size; i++)
    {
        if (population_crossover->individuos[i].fitness < population_original->individuos[i].fitness)
            population_original->individuos[i] = population_crossover->individuos[i];
    }
    qsort(population_original->individuos, population_original->size, sizeof(individuo), comparador_individuo);
    DEBUG(printf("População selecionada\n"););
    DEBUG(print_population(population_original->individuos, population_original->size, dimension, 1););
    return *population_original;
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

populacao *crossover(populacao *populacao_, int dimension)
{
    populacao *nova_populacao = generate_island(1, populacao_->size, dimension, (domain){bounds_lower, bounds_upper});
    DEBUG(printf("\ncruzamento\n"););
    for (int i = 0; i < populacao_->size; i++)
    {
        individuo *parents[2];
        select_parents(*populacao_, parents);

        individuo parent1 = *parents[0];
        individuo parent2 = *parents[1];
        individuo filho;
        switch (populacao_->crossover)
        {
        case MEDIA:
            filho = cruzamento_media(parent1, parent2, dimension);
        case METADE:
            filho = cruzamento_metade(parent1, parent2, dimension);
        case PONTO:
            filho = cruzamento_ponto(parent1, parent2, dimension);
        case MEDIA_GEOMETRICA:
            filho = cruzamento_ponto(parent1, parent2, dimension);
        case FLAT:
            filho = cruzamento_flat(parent1, parent2, dimension);
        case BLEND:
            filho = cruzamento_blend(parent1, parent2, dimension);
        default:
            filho = cruzamento_media(parent1, parent2, dimension);
        }

        fitness(&filho, dimension);
        nova_populacao->individuos[i] = filho;
    }
    // nova_populacao->individuos[populacao->size - 1] = populacao->individuos[populacao->size - 1];
    return nova_populacao;
}

void migrate(populacao *populations, int island_size, int dimension, domain domain_function)
{
    DEBUG(printf("\nmigrate\n"););
    populacao *vizinho;

    for (int i = 0; i < island_size; i++)
    {
        DEBUG(printf("Populacao %d\n", i));
        populacao current_island = populations[i];
        individuo *population = current_island.individuos;
        individuo *melhor_individuo_da_populacao = get_best_of_population(current_island);
        DEBUG(printf("Melhor individuo da populacao %d: %lf\n", i, melhor_individuo_da_populacao->fitness););
        for (int j = 0; j < 4; j++)
        {
            vizinho = current_island.neighbours[j];
            if (vizinho == NULL)
                continue;
            individuo *neighbour_population = vizinho->individuos;
            individuo *pior_indivuduo_do_vizinho = get_worst_of_population(neighbour_population, population_size);
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

individuo evolution(int island_size, int population_size, int dimension, domain domain_function, int num_generations)
{
    DEBUG(printf("\nevolution\n"););
    individuo *parents[2];
    individuo bestIndividuo = {.fitness = INFINITY};
    populacao *populations = generate_island(island_size, population_size, dimension, domain_function);
    time_t time_init, time_now;

    time(&time_init);
    time(&time_now);
    DEBUG(printf("Iniciando evolucao\n"););
    while (difftime(time_now, time_init) < time_limit)
    {
        for (int i = 0; i < island_size; i++)
        {
            DEBUG(printf("\n\ni-ésima ilha: %d\n", i););
            populacao *current_population = &populations[i];
            populacao *cross_population = current_population;
            int generation_count = 0;
            while (generation_count < num_generations)
            {
                mutation_commom(current_population, dimension, domain_function);
                cross_population = crossover(current_population, dimension);
                *current_population = selection(current_population, cross_population, dimension);

                //print_individuo(current_population->individuos[current_population->size - 1], dimension);
                generation_count++;
                STATISTICS(print_coords(&current_population->individuos[current_population->size - 1], 1, generation_count, num_generations););
                DEBUG(printf("\nGeração: %d\n", generation_count););
            }
            individuo *bestCurrent = get_best_of_population(*current_population);

            // puts("\nMelhor de toda a população:");
            // print_individuo(*bestCurrent, dimension);
            // printf("%lf\n", bestCurrent->fitness);
            if (bestCurrent->fitness < bestIndividuo.fitness)
                bestIndividuo = *bestCurrent;
        }
        migrate(populations, island_size, dimension, domain_function);
        time(&time_now);
    }
    return bestIndividuo;
}

int main(int argc, char *argv[])
{
    set_parameters(argc, argv); // Lê os parâmetros da linha de comando e repassa para as variáveis globais
    // print_parameters();

    time_t semente = 10;
    printf("Semente: %ld\n ", semente);
    individuo result;
    // Melhor semente até agora: 1676931005 (Funcao 3) - 301.356
    // Melhor semente até agora: 1676935665 (Funcao 8) - 801.1393
    srand(semente);
    result = evolution(island_size, population_size, dimension, (domain){bounds_lower, bounds_upper}, num_generations);

    print_individuo(result, dimension);
    return 0;
}