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
#define DEBUG(x)
#define LOG(x)

static args parameters;

void print_usage()
{
    printf("Usage: ./evolucao_mpop -f <function_number> -t <time_limit> -i <island_size> -p <population_size> -d <dimension> -l <bounds_lower> -u <bounds_upper> -g <num_generations> -m <mutation_probability>");
}

void set_default_parameters()
{
    parameters.F = 0.99;
    parameters.function_number = 1;
    parameters.time_limit = 10; // seconds
    parameters.island_size = 10;
    parameters.population_size = 5598;
    parameters.dimension = 10; // 10 or 30
    parameters.domain_function.min = -100;
    parameters.domain_function.max = 100;
    parameters.num_generations_per_epoca = 300;
    parameters.mutation_rate = 100;  // %
    parameters.crossover_rate = 100; // %
    parameters.num_migrations = 3;
    parameters.seed = time(NULL);
    parameters.c1 = 1.523;
    parameters.c2 = 0.77623;
}

void set_parameters(int argc, char *argv[])
{
    int opt;
    set_default_parameters();
    while ((opt = getopt(argc, argv, "f:F:t:i:p:d:l:u:g:m:c:k:s:v:z:")) != -1)
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
        case 'v':
            parameters.c1 = atof(optarg);
            break;
        case 'z':
            parameters.c2 = atof(optarg);
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

individuo *generate_population(int n_individuos, int dimension, domain domain_function)
{
    DEBUG(printf("\ngenerate_population\n"););
    individuo *population = malloc(n_individuos * sizeof(individuo));
    for (int i = 0; i < n_individuos; i++)
    {
        population[i].chromosome = (double *)malloc(dimension * sizeof(double));
        population[i].velocidade = (double *)malloc(dimension * sizeof(double));
        for (int j = 0; j < dimension; j++)
        {
            population[i].chromosome[j] = random_double(domain_function.min, domain_function.max);
            population[i].velocidade[j] = 0;
        }
        fitness(&population[i], dimension);
    }
    return population;
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

void copy_individuo(individuo *original, individuo *copia, int dimension)
{
    copia->fitness = original->fitness;
    for (int i = 0; i < dimension; i++)
    {
        copia->chromosome[i] = original->chromosome[i];
        copia->velocidade[i] = original->velocidade[i];
    }
}

void verifica_limites(individuo *individuo, int dimension)
{
    double vmax = pow(parameters.domain_function.max, 2) * 0.002;
    double vmin = -1 * vmax;
    // printf("\nvmax: %lf,   vmin: %lf\n", vmax,vmin);
    for (int i = 0; i < dimension; i++)
    {
        if (individuo->chromosome[i] < parameters.domain_function.min)
        {
            individuo->chromosome[i] = parameters.domain_function.min;
            individuo->velocidade[i] = vmin;
        }
        else if (individuo->chromosome[i] > parameters.domain_function.max)
        {
            individuo->chromosome[i] = parameters.domain_function.max;
            individuo->velocidade[i] = vmax;
        }
    }
}

void atualiza_posicao(individuo *individuo_atual, int dimension)
{ // puts("Individuo antes da atualização:");
    // print_individuo(*individuo_atual, dimension, 0);
    for (int i = 0; i < dimension; i++)
    {
        individuo_atual->chromosome[i] = individuo_atual->chromosome[i] + individuo_atual->velocidade[i];
    }
    // puts("Individuo depois da atualização");
    // print_individuo(*individuo_atual, dimension, 0);
    verifica_limites(individuo_atual, dimension);
}

void calcula_componente(double *componente, individuo *individuo_1, individuo *individuo_2, int dimension)
{
    for (int i = 0; i < dimension; i++)
    {
        componente[i] = individuo_1->chromosome[i] - individuo_2->velocidade[i];
        // printf("%lf  ", componente[i]);
        // printf("\n");
    }
}

individuo pso()
{
    populacao *population = malloc(sizeof(population));
    populacao *population_best_current = malloc(sizeof(population));
    population->individuos = generate_population(parameters.population_size, parameters.dimension, parameters.domain_function);
    population_best_current->individuos = generate_population(parameters.population_size, parameters.dimension, parameters.domain_function);
    population->size = parameters.population_size;
    time_t time_init, time_now;
    time(&time_init);
    time(&time_now);

    individuo *individuo_best = generate_population(1, parameters.dimension, parameters.domain_function);
    double w_max = 1, w_min = 0;
    double *c1 = malloc(parameters.dimension * sizeof(double));
    double *c2 = malloc(parameters.dimension * sizeof(double));
    int generation_count = 0;
    int max_inter_add = 100;
    int max_inter = max_inter_add;
    int cont_or_stop = 1;
    while (cont_or_stop && difftime(time_now, time_init) < parameters.time_limit)
    {
        double best_anter = individuo_best->fitness;
        while (generation_count < max_inter && difftime(time_now, time_init) < parameters.time_limit)
        {
            for (int i = 0; i < parameters.population_size; i++)
            {
                if (population->individuos[i].fitness < population_best_current->individuos[i].fitness)
                    copy_individuo(&population->individuos[i], &population_best_current->individuos[i], parameters.dimension);
                if (population->individuos[i].fitness < individuo_best->fitness)
                    copy_individuo(&population->individuos[i], individuo_best, parameters.dimension);
                double r1 = (double)rand() / RAND_MAX;
                double r2 = (double)rand() / RAND_MAX;
                // calcula_componente(c1, &population_best_current->individuos[i], &population->individuos[i], parameters.dimension);
                // calcula_componente(c2, individuo_best, &population->individuos[i], parameters.dimension);

                // w(t) = w_max - ((w_max - w_min) * t) / T
                double w = w_max - ((w_max - w_min) * generation_count) / max_inter;
                for (int j = 0; j < parameters.dimension; j++)
                {
                    // v_{i,d}(t+1) = wv_{i,d}(t) + c1r1*(pbest_{i,d}-x_{i,d}(t)) + c2r2(gbest_{d}-x_{i,d}(t))
                    population->individuos[i].velocidade[j] = population->individuos[i].velocidade[j] * w +
                                                              parameters.c1 * r1 * (population_best_current->individuos[i].chromosome[j] - population->individuos[i].chromosome[j]) +
                                                              parameters.c2 * r2 * (individuo_best->chromosome[j] - population->individuos[i].chromosome[j]);
                }

                atualiza_posicao(&population->individuos[i], parameters.dimension);
                fitness(&population->individuos[i], parameters.dimension);
                // print_individuo(population->individuos[i], dimension, 0);
            }
            time(&time_now);

            generation_count++;
        }
        // double desv = desvio_padrao(population->individuos, population_size);
        // printf("Desvio_P: %lf\n", desv);
        if (doubleEqual(best_anter, individuo_best->fitness, 4))
        {
            cont_or_stop = 0;
        }
        else
        {
            max_inter += max_inter_add;
        }
    }
    return *individuo_best;
}

int main(int argc, char *argv[])
{
    set_parameters(argc, argv); // Lê os parâmetros da linha de comando e repassa para as variáveis globais
    // print_parameters();

    individuo result;
    // Melhor semente até agora: 1676931005 (Funcao 3) - 301.356
    // Melhor semente até agora: 1676935665 (Funcao 8) - 801.1393
    srand(parameters.seed);
    // result = evolution(parameters.island_size, parameters.population_size, parameters.dimension, parameters.domain_function, parameters.num_generations_per_epoca);
    result = pso();

    print_individuo(result, parameters.dimension, 0);
    printf("Best %lf\n", result.fitness);
    return 0;
}