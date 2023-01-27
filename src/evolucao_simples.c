#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "./funcoes_cec_2015/cec15_test_func.h"

#define DEBUG(x) 

typedef struct individue_
{
    double *chromosome;
    double fitness;
} individue;

typedef struct domain {
    double min;
    double max;
} domain;


int print_individue(individue individuo, int dimension)
{
    DEBUG(printf("\nprint_individue\n"););
    printf("Individue: ");
    for (int j = 0; j < dimension; j++)
    {
        printf("%lf ", individuo.chromosome[j]);
    }
    printf("Fitness: %lf\n", individuo.fitness);
    return 0;
}

int print_population(individue *population, int n_populacoes, int dimension)
{
    DEBUG(printf("\nprint_population\n"););
    for (int i = 0; i < n_populacoes; i++)
    {
        print_individue(population[i], dimension);
    }
    return 0;
}

double random_double(double min, double max)
{
    double random = min + (max - min) * (rand() / (double)RAND_MAX);
    return random;
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

void select_parents(individue *populacao, int n_populacoes, int *parents)
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

    int percent_ant = 0;
    for (int i = 0; i < n_populacoes; i++)
    {
        double individue_beneficio = populacao[i].fitness;
        DEBUG(printf("individue_beneficio: %lf\n", individue_beneficio););
        int percent = ceil((double)(individue_beneficio) / (double)sum_beneficio * 100.00);
        DEBUG(printf("Preenchendo roleta com %d de %d até %d\n", i, percent_ant, percent_ant + percent););
        for (int j = percent_ant; j < percent_ant + percent; j++)
        {
            roulette[j] = i;
        }
        percent_ant += percent;
    }

    int ball1 = rand() % percent_ant;
    int ball2;
    do
    {
        ball2 = rand() % percent_ant;
        if (ball1 != ball2)
        {
            parents[0] = roulette[ball1];
            parents[1] = roulette[ball2];
            break;
        }
    } while (1);
    DEBUG(print_roleta(roulette, percent_ant, ball1, ball2););
    DEBUG(printf("select_parents: %d e %d\n", parents[0], parents[1]););
    //exit(0);
}

typedef enum {
    MEDIA,
    MEDIA_GEOMETRICA,
    METADE,
    PONTO,
    FLAT,
    BLEND,
} crossover_type;



individue cruzamento_media(individue parent1, individue parent2, int n_itens) {
    DEBUG(printf("\ncruzamento_media\n"););

    double *child_chromosome = (double *) malloc(n_itens * sizeof(double));
    individue child = {child_chromosome, INFINITY};

    for (int i = 0; i < n_itens; i++)
    {
        child.chromosome[i] = (parent1.chromosome[i] + parent2.chromosome[i]) / 2;
    }
    fitness(&child, n_itens);
    return child;
}

individue cruzamento_media_g(individue parent1, individue parent2, int n_itens) {
    DEBUG(printf("\ncruzamento_media\n"););

    double *child_chromosome = (double *) malloc(n_itens * sizeof(double));
    individue child = {child_chromosome, INFINITY};

    for (int i = 0; i < n_itens; i++)
    {
        child.chromosome[i] = sqrt(parent1.chromosome[i] * parent2.chromosome[i]);
    }
    fitness(&child, n_itens);
    return child;
}

individue cruzamento_flat(individue parent1, individue parent2, int n_itens) {
    DEBUG(printf("\ncruzamento_flat\n"););

    double *child_chromosome = (double *) malloc(n_itens * sizeof(double));
    individue child = {child_chromosome, INFINITY};

    for (int i = 0; i < n_itens; i++)
    {
        individue pais[2] = {parent1, parent2};
        int menor = parent1.chromosome[i] < parent2.chromosome[i] ? 0 : 1;
        child.chromosome[i] = random_double(pais[menor].chromosome[i], pais[!menor].chromosome[i]);
    }
    fitness(&child, n_itens);
    return child;
}

individue cruzamento_blend(individue parent1, individue parent2, int n_itens) {
    DEBUG(printf("\ncruzamento_blend\n"););

    double *child_chromosome = (double *) malloc(n_itens * sizeof(double));
    individue child = {child_chromosome, INFINITY};

    for (int i = 0; i < n_itens; i++)
    {
        double alpha = random_double(0.0, 1.0);
        child.chromosome[i] = alpha * parent1.chromosome[i] + (1 - alpha) * parent2.chromosome[i];
    }
    fitness(&child, n_itens);
    return child;
}

individue cruzamento_metade(individue parent1, individue parent2, int n_itens) {
    DEBUG(printf("\ncruzamento_metade\n"););

    double *child_chromosome = (double *) malloc(n_itens * sizeof(double));
    individue child = {child_chromosome, INFINITY};

    int crossover_point = n_itens / 2;
    for (int i = 0; i < crossover_point; i++)
    {
        child.chromosome[i] = parent1.chromosome[i];
    }
    for (int i = crossover_point; i < n_itens; i++)
    {
        child.chromosome[i] = parent2.chromosome[i];
    }
    fitness(&child, n_itens);
    return child;
}

individue cruzamento_ponto(individue parent1, individue parent2, int n_itens) {
    DEBUG(printf("\ncruzamento_metade\n"););

    double *child_chromosome = (double *) malloc(n_itens * sizeof(double));
    individue child = {child_chromosome, INFINITY};

    int crossover_point = rand() % n_itens;
    for (int i = 0; i < crossover_point; i++)
    {
        child.chromosome[i] = parent1.chromosome[i];
    }
    for (int i = crossover_point; i < n_itens; i++)
    {
        child.chromosome[i] = parent2.chromosome[i];
    }
    fitness(&child, n_itens);
    return child;
}

individue cruzamento(individue parent1, individue parent2, int n_itens)
{
    DEBUG(printf("\ncruzamento\n"););

    crossover_type crossover = PONTO;
    switch (crossover)
    {
    case MEDIA:
        return cruzamento_media(parent1, parent2, n_itens);
    case METADE:
        return cruzamento_metade(parent1, parent2, n_itens);
    case PONTO:
        return cruzamento_ponto(parent1, parent2, n_itens);
    case MEDIA_GEOMETRICA:
        return cruzamento_media_g(parent1, parent2, n_itens);
    case FLAT:
        return cruzamento_flat(parent1, parent2, n_itens);
    case BLEND:
        return cruzamento_blend(parent1, parent2, n_itens);
    default:
        return cruzamento_media(parent1, parent2, n_itens);
    }
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

void variation_populations(individue *populacao, int n_populacoes, int dimension, domain domain_function)
{
    DEBUG(printf("\nvariation_populations\n"););
    for (int i = 0; i < n_populacoes; i++)
    {
        
    }
}

individue *generate_population(int n_populacoes, int dimension, domain domain_function)
{
    DEBUG(printf("\ngenerate_population\n"););
    individue *population = malloc(n_populacoes * sizeof(individue));
    for (int i = 0; i < n_populacoes; i++)
    {
        population[i].chromosome = (double *) malloc(dimension * sizeof(double));
        for (int j = 0; j < dimension; j++)
        {
            population[i].chromosome[j] = random_double(domain_function.min, domain_function.max);
        }
        population[i].fitness = INFINITY;
    }

    //DEBUG(print_population(population, n_populacoes, dimension););
    return population;
}



individue mutation(individue individuo, int dimension, domain domain_function)
{
    DEBUG(printf("\nmutation\n"););
    int mutation_point = rand() % dimension;
    individuo.chromosome[mutation_point] = random_double(domain_function.min, domain_function.max);
    individuo.fitness = INFINITY;
    return individuo;
}

individue *get_best_of_population(individue *population, int n_populacoes)
{
    qsort(population, n_populacoes, sizeof(individue), comparador_individue);
    return &population[n_populacoes - 1];
}

void fitness(individue *individuo, int dimension)
{
    //individuo.fitness = real_function(individuo.chromosome, dimension);
    cec15_test_func(individuo->chromosome, &individuo->fitness, dimension, 1, 3);
    //printf("fitness: %f\n", individuo->fitness);
}

int selection(individue *population, int n_populacoes, int dimension)
{
    DEBUG(printf("\nselection\n"););
    for (int i = 0; i < n_populacoes; i++)
    {
        if (population[i].fitness == INFINITY)
            fitness(&population[i], dimension);
    }

    //print_population(population, n_populacoes, dimension);

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
individue *evolution(int population_size, int dimension, domain domain_function, double select_criteria)
{
    DEBUG(printf("\nevolution\n"););
    int parents[2];
    individue *population = generate_population(population_size, dimension, domain_function);
    // int generations_count = 0;
    time_t time_init, time_now;
    int generations_count = 0;
    time(&time_init);
    do
    {
        // printf("\ni-ésima geração: %d\n", generations_count);
        selection(population, population_size, dimension);
        //print_population(population, population_size, dimension);
        for (int i = 0; i < (population_size * 90) / 100; i++)
        {

            DEBUG(printf("\ni-ésimo individuo: %d\n", i););

            select_parents(population, population_size, parents);
            individue child = cruzamento(population[parents[0]], population[parents[1]], dimension);
            if (in_fitness_population(population, population_size, child))
            {
                child = mutation(child, dimension, domain_function);
                fitness(&child, dimension);
            }
            DEBUG(printf("Custo do filho: %lf\n", child.fitness););
            // free(&population[i]);
            if (child.fitness < population[i].fitness)
                population[i] = child;
        }

        for (int i = 0; i < population_size - 1; i++)
        {
            if (rand() % 100 < 10)
            {
                population[i] = mutation(population[i], dimension, domain_function);
            }
        }
        // print_population(population, population_size, itens, n_itens);
        time(&time_now);
        generations_count++;
    printf("Geração: %d\n", generations_count);
    } while (!avaliar(population, population_size, select_criteria) && difftime(time_now, time_init) < 10 && generations_count < 500);

    return get_best_of_population(population, population_size);
}

int main(int argc, char *argv[])
{

    individue *result = NULL;
    result = evolution(100, 10, (domain){-100, 100}, 10);

    print_individue(*result, 10);
    return 0;
}