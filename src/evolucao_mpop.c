#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#include "./libs/funcoes_cec_2015/cec15_test_func.h"
#include "./libs/statistics.h"
#include "./libs/types.h"
#include "./libs/utils.h"
#include "./libs/crossover.h"
#define STATISTICS(x)
#define DEBUG(x)

#define ISLAND_SIZE 10
#define POPULATION_SIZE 3
#define NUM_GENERATIONS 300
#define MUTATION_PROBABILITY 80 // %
#define DIMENSION 10            // 10 or 30
#define BOUNDS_LOWER -100
#define BOUNDS_UPPER 100
#define SELECT_CRITERIA 0.0001
#define FUNCTION_NUMBER 1 // 1 to 15

#define TIME_LIMIT 10 // seconds

static int function_number = 1;
static int time_limit = 10; // seconds
static int island_size = 10;
static int population_size = 3;
static int dimension = 10; // 10 or 30
static int bounds_lower = -100;
static int bounds_upper = 100;
static int num_generations = 300;
static int mutation_probability = 80; // %

void print_usage()
{
    printf("Usage: ./evolucao_mpop -f <function_number> -t <time_limit> -i <island_size> -p <population_size> -d <dimension> -l <bounds_lower> -u <bounds_upper> -g <num_generations> -m <mutation_probability>");
}

void set_parameters(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "f:t:i:p:d:l:u:g:m:")) != -1)
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
    printf(" function_number: %d,\n time_limit: %d,\n island_size: %d,\n population_size: %d,\n dimension: %d,\n bounds_lower: %d,\n bounds_upper: %d,\n num_generations: %d,\n mutation_probability: %d\n", function_number, time_limit, island_size, population_size, dimension, bounds_lower, bounds_upper, num_generations, mutation_probability);
}

void fitness(individue *individuo, int dimension)
{
    // individuo.fitness = real_function(individuo.chromosome, dimension);
    cec15_test_func(individuo->chromosome, &individuo->fitness, dimension, 1, function_number);
    // double x = individuo->chromosome[0];
    // double y = individuo->chromosome[1];
    // individuo->fitness = pow(x, 2) + pow(y, 2) - cos(18 * x) - cos(18 * y);
    // printf("fitness: %f\n", individuo->fitness);
}

int comparador_individue(const void *a, const void *b)
{
    individue *v1 = (individue *)a;
    individue *v2 = (individue *)b;

    return v1->fitness < v2->fitness;
}

int avaliar(individue *populacao, int n_populacoes, int select_criteria)
{
    int count = 0;
    DEBUG(printf("\navaliar\n"););
    for (int i = 0; i < n_populacoes; i++)
    {
        double individue_custo = populacao[i].fitness;
        if (individue_custo < select_criteria)
        {
            count += 1;
        }
    }

    float percent = (float)count / (float)n_populacoes;
    return percent > 0.90;
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

int roleta_pais(individue *populacao, int n_populacoes)
{
    DEBUG(printf("\nselect_parents\n"););
    int roulette[100000];
    double sum_beneficio = 0.0;

    DEBUG(printf("sum_beneficio[init]\n"););
    for (int i = 0; i < n_populacoes; i++)
    {
        sum_beneficio += populacao[i].fitness;
    }
    DEBUG(printf("sum_beneficio[end]: %lf\n", sum_beneficio););

    int base = 0;
    for (int i = 0; i < n_populacoes; i++)
    {
        double individue_beneficio = sum_beneficio - populacao[i].fitness;
        DEBUG(printf("individue_beneficio: %lf\n", individue_beneficio););
        int limit = ceil((double)(individue_beneficio) / (double)sum_beneficio * 100.00);
        DEBUG(printf("Preenchendo roleta com %d de %d até %d\n", i, base, base + limit););
        for (int j = base; j < base + limit; j++)
        {
            roulette[j] = i;
        }
        base += limit;
    }

    return roulette[rand() % base];
}

void select_parents(individue *populacao, int n_populacoes, individue *parents[2])
{
    int pai1 = roleta_pais(populacao, n_populacoes), pai2;
    do
    {
        pai2 = roleta_pais(populacao, n_populacoes);
    } while (pai1 == pai2);
    parents[0] = &populacao[pai1];
    parents[1] = &populacao[pai2];
    // printf("Pais: %d e %d\n", pai1, pai2);
}

individue cruzamento(individue *parents[2], int n_itens, int crossover)
{
    DEBUG(printf("\ncruzamento\n"););
    individue parent1 = *parents[0];
    individue parent2 = *parents[1];
    individue filho;
    switch (crossover)
    {
    case MEDIA:
        filho = cruzamento_media(parent1, parent2, n_itens);
    case METADE:
        filho = cruzamento_metade(parent1, parent2, n_itens);
    case PONTO:
        filho = cruzamento_ponto(parent1, parent2, n_itens);
    case MEDIA_GEOMETRICA:
        filho = cruzamento_ponto(parent1, parent2, n_itens);
    case FLAT:
        filho = cruzamento_flat(parent1, parent2, n_itens);
    case BLEND:
        filho = cruzamento_blend(parent1, parent2, n_itens);
    default:
        filho = cruzamento_media(parent1, parent2, n_itens);
    }

    fitness(&filho, n_itens);
    return filho;
}

int in_fitness_population(individue *populacao, int n_populacoes, individue individuo)
{
    DEBUG(printf("\nin_fitness_population\n"););
    for (int i = 0; i < n_populacoes; i++)
    {
        if (populacao[i].fitness == individuo.fitness)
        {
            return 1;
        }
    }
    return 0;
}

individue *generate_population(int n_individuos, int dimension, domain domain_function)
{
    DEBUG(printf("\ngenerate_population\n"););
    individue *population = malloc(n_individuos * sizeof(individue));
    for (int i = 0; i < n_individuos; i++)
    {
        population[i].chromosome = (double *)malloc(dimension * sizeof(double));
        for (int j = 0; j < dimension; j++)
        {
            population[i].chromosome[j] = random_double(domain_function.min, domain_function.max);
        }
        population[i].fitness = INFINITY;
    }
    return population;
}

individue mutation(individue individuo, int dimension, domain domain_function)
{
    DEBUG(printf("\nmutation\n"););
    int mutation_point = rand() % dimension;
    individuo.chromosome[mutation_point] = random_double(domain_function.min, domain_function.max);
    // individuo.chromosome[mutation_point] = !individuo.chromosome[mutation_point];
    fitness(&individuo, dimension);

    return individuo;
}

individue *get_best_of_population(individue *population, int n_populacoes)
{
    DEBUG(printf("\nget_best_of_population\n"););
    // qsort(population, n_populacoes, sizeof(individue), comparador_individue);
    return &population[n_populacoes - 1];
}

individue *get_worst_of_population(individue *population, int n_populacoes)
{
    // qsort(population, n_populacoes, sizeof(individue), comparador_individue);
    return &population[0];
}

int selection(individue *population, int n_populacoes, int dimension)
{
    DEBUG(printf("\nselection\n"););
    for (int i = 0; i < n_populacoes; i++)
    {
        if (population[i].fitness == INFINITY)
            fitness(&population[i], dimension);
    }

    qsort(population, n_populacoes, sizeof(individue), comparador_individue);
    return 0;
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

individue *get_pior_pai(individue *pais[2])
{
    return pais[0]->fitness < pais[1]->fitness ? pais[1] : pais[0];
}

population *generate_island(int island_size, int population_size, int dimension, domain domain_function)
{
    DEBUG(printf("\ngenerate_island\n"););
    population *populations = malloc(island_size * sizeof(population));
    population **neighbours = calloc(4, sizeof(population *));
    for (int i = 0; i < island_size; i++)
    {
        populations[i].individues = generate_population(population_size, dimension, domain_function);
        populations[i].size = population_size;
        populations[i].crossover = rand() % 6;
        populations[i].neighbours = calloc(4, sizeof(population *));
        populations[i].neighbours[0] = &populations[(i + 1) % island_size]; // talvez isso dê problema
        populations[i].neighbours[1] = &populations[(i + 3) % island_size]; // talvez isso dê problema
    }
    return populations;
}

void migrate(population *populations, int island_size, int population_size, int dimension, domain domain_function)
{
    DEBUG(printf("\nmigrate\n"););
    population *vizinho;

    for (int i = 0; i < island_size; i++)
    {
        DEBUG(printf("Populacao %d\n", i));
        population current_island = populations[i];
        individue *population = current_island.individues;
        individue *melhor_individuo_da_populacao = get_best_of_population(population, population_size);
        DEBUG(printf("Melhor individuo da populacao %d: %lf\n", i, melhor_individuo_da_populacao->fitness););
        for (int j = 0; j < 4; j++)
        {
            vizinho = current_island.neighbours[j];
            if (vizinho == NULL)
                continue;
            individue *neighbour_population = vizinho->individues;
            individue *pior_indivuduo_do_vizinho = get_worst_of_population(neighbour_population, population_size);
            if (melhor_individuo_da_populacao->fitness > pior_indivuduo_do_vizinho->fitness)
            {
                individue *new_worst = generate_population(1, dimension, domain_function);
                *pior_indivuduo_do_vizinho = *new_worst;
            }
            else
                *pior_indivuduo_do_vizinho = *melhor_individuo_da_populacao;
        }
    }
}

individue evolution(int island_size, int population_size, int dimension, domain domain_function, double select_criteria, int num_generations)
{
    DEBUG(printf("\nevolution\n"););
    individue *parents[2];
    individue bestIndividuo = {.fitness = INFINITY};
    population *populations = generate_island(island_size, population_size, dimension, domain_function);
    // int generations_count = 0;
    time_t time_init, time_now;

    time(&time_init);
    time(&time_now);
    DEBUG(printf("Iniciando evolucao\n"););
    while (difftime(time_now, time_init) < time_limit)
    {
        for (int i = 0; i < island_size; i++)
        {
            DEBUG(printf("\n\ni-ésima ilha: %d\n", i););
            population *current_island = &populations[i];
            individue *population = current_island->individues;
            int generation_count = 0;
            while (generation_count < num_generations)
            {
                selection(population, population_size, dimension);

                for (int i = 0; i < current_island->size - 1; i++)
                {

                    DEBUG(printf("\ni-ésimo individuo: %d\n", i););

                    select_parents(population, population_size, parents);
                    individue child = cruzamento(parents, dimension, current_island->crossover);
                    // O if abaixo garante que nunca haverá dois individuos iguais na população
                    // if (in_fitness_population(population, population_size, child))
                    // {
                    //     child = mutation(child, dimension, domain_function);
                    // }
                    DEBUG(printf("Custo do filho: %lf\n", child.fitness););
                    individue *pior_pai = get_pior_pai(parents);
                    *pior_pai = child;

                    if (rand() % 100 < MUTATION_PROBABILITY)
                    {
                        population[i] = mutation(population[i], dimension, domain_function);
                    }
                }
                selection(population, population_size, dimension);
                generation_count++;
                STATISTICS(print_coords(population, population_size, generation_count, num_generations););
            }
            individue *bestCurrent = get_best_of_population(population, population_size);
            if (bestCurrent->fitness < bestIndividuo.fitness)
                bestIndividuo = *bestCurrent;
        }
        migrate(populations, island_size, population_size, dimension, domain_function);
        time(&time_now);
    }
    DEBUG(printf("Geração: %d\n", generations_count););
    return bestIndividuo;
}

int main(int argc, char *argv[])
{
    set_parameters(argc, argv); // Lê os parâmetros da linha de comando e repassa para as variáveis globais
    print_parameters();

    time_t semente = time(NULL);
    printf("Semente: %ld\n ", semente);
    individue result;
    // Melhor semente até agora: 1676931005 (Funcao 3) - 301.356
    // Melhor semente até agora: 1676935665 (Funcao 8) - 801.1393
    srand(semente);
    result = evolution(island_size, population_size, dimension, (domain){bounds_lower, bounds_upper}, SELECT_CRITERIA, num_generations);

    print_individue(result, DIMENSION);
    return 0;
}