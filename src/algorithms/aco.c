#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "aco.h"
#include "parameters.h"
#include "commom.h"

#define DEBUG(x)

double *sigma;
double **pheromones_candidates;

individuo *candidates;

void set_default_parameters_ant()
{
    if (!parameters.num_ants)
        parameters.num_ants = 5252; // 5252;
    if (!parameters.num_generations_per_epoca)
        parameters.num_generations_per_epoca = 2000;
    if (!parameters.tax_evaporate)
        parameters.tax_evaporate = 0.31458;
    if (!parameters.num_candidates)
        parameters.num_candidates = 9;
    if (!parameters.p_exploitation)
        parameters.p_exploitation = 0.25;
    if (!parameters.function_number)
        parameters.function_number = 5;
    if (!parameters.time_limit)
        parameters.time_limit = 10; // in seconds
    if (!parameters.seed)
        parameters.seed = time(NULL);
    if (!parameters.dimension)
        parameters.dimension = 10; // 10 or 30
    if (!parameters.domain_function.min)
        parameters.domain_function.min = -100;
    if (!parameters.domain_function.max)
        parameters.domain_function.max = 100;
    srand(parameters.seed);
}

void reset_parameters_ant()
{
    parameters.num_ants = 0;
    parameters.num_generations_per_epoca = 0;
    parameters.tax_evaporate = 0;
    parameters.num_candidates = 0;
    parameters.p_exploitation = 0;
    parameters.seed = 0;
}

void print_individuos(individuo *individuo, int dimension)
{
    for (int i = 0; i < parameters.num_ants; i++)
    {
        print_individuo(*individuo, dimension, i);
    }
}

// Initialize the individuos' positions randomly
void initialize(individuo *individuos, int n, int d)
{
    DEBUG(printf("Initializing the individuos\n");)
    for (int i = 0; i < n; i++)
    {
        individuos[i].chromosome = (double *)malloc(d * sizeof(double));
        for (int j = 0; j < d; j++)
        {
            individuos[i].chromosome[j] = random_double(-100, 100);
        }
        fitness(&individuos[i], d, parameters.function_number);
    }
}
void evaporate_pheromones(double **pheromones, int dimension)
{
    for (int i = 0; i < parameters.num_ants; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            pheromones[i][j] = pheromones[i][j] * (1 - parameters.tax_evaporate);
        }
    }
}

// Update the pheromone information in the positions visited by the individuos
void update_pheromones(double **pheromones, individuo *individuos, int n, int d, individuo *best_individuo)
{
    DEBUG(printf("update_pheromones\n");)

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < d; j++)
        {
            pheromones[i][j] = (1 / sqrt(2 * sigma[j] * PI)) * exp(pow((best_individuo->chromosome[j] - individuos[i].chromosome[j]), 2) / (-2 * pow(sigma[j], 2)));
            if (j % 2 == 0)
                pheromones[i][j] += random_double(0, 0.3);
        }
    }
}

double sum_individuo_dimension(individuo *individuos, int current_d)
{
    double sum = 0.0;
    for (int i = 0; i < parameters.num_ants; i++)
    {
        sum += individuos[i].chromosome[current_d];
    }
    return sum;
}

double sum_pheromone_dimension(double **pheromones, int d)
{
    double sum = 0.0;
    for (int i = 0; i < parameters.num_ants; i++)
    {
        sum += pheromones[i][d];
    }
    return sum;
}

double sum_pheromone_individuo(double **pheromones, int d, int id)
{
    double sum = 0.0;
    for (int i = 0; i < d; i++)
    {
        sum += pheromones[id][i];
    }
    return sum;
}

double sum_pheromone_individuos(double **pheromones, int d, int n_individuos)
{
    double sum = 0.0;
    for (int j = 0; j < n_individuos; j++)
    {
        for (int i = 0; i < d; i++)
        {
            sum += pheromones[j][i];
        }
    }
    return sum;
}

void select_next_position2(double **pheromones, individuo *individuos, int d)
{
    DEBUG(printf("select_next_position\n");)
    double sum_pheromone[d], prob;

    for (int i = 0; i < d; i++)
    {
        sum_pheromone[i] = sum_pheromone_dimension(pheromones, i);
    }

    for (int i = 0; i < parameters.num_ants; i++)
    {
        for (int j = 0; j < d; j++)
        {
            prob = pheromones[i][j] / sum_pheromone[j];

            if (((double)rand() / RAND_MAX) < prob)
            {
                individuos[i].chromosome[j] += random_double(-5, 5);
            }
        }
        fitness(&individuos[i], d, parameters.function_number);
    }
}

void saved_chosen(individuo *individuos, int id_origin, int id_new, int d)
{
    for (int i = 0; i < d; i++)
    {
        individuos[id_origin].chromosome[i] = candidates[id_new].chromosome[i];
    }
}

void candiate_calculator_crossover(individuo *individuos, int d, int id_individuo, int id_candit)
{
    double current_dimension_value;
    int individuo2 = rand() % parameters.num_ants;

    while (individuo2 == id_individuo)
        individuo2 = rand() % parameters.num_ants;

    for (int j = 0; j < d; j++)
    {
        current_dimension_value = individuos[id_individuo].chromosome[j];

        candidates[id_candit].chromosome[j] = (current_dimension_value + individuos[individuo2].chromosome[j]) / 2;
        if (random_double(0, 1) <= parameters.p_exploitation || !(candidates[id_candit].chromosome[j] <= 100 && candidates[id_candit].chromosome[j] >= -100))
            candidates[id_candit].chromosome[j] = random_double(-100, 100);
    }
}

void candiate_calculator(individuo *individuos, double **pheromones, double *sum_individuo_dimension, int d, int id_individuo, int id_candit)
{
    double distance, current_pheromone, current_dimension_value, delta, mean;
    for (int j = 0; j < d; j++)
    {
        current_pheromone = pheromones[id_individuo][j];
        current_dimension_value = individuos[id_individuo].chromosome[j];
        mean = (sum_individuo_dimension[j] - current_dimension_value) / parameters.num_ants - 1;
        distance = abs_double(mean - current_dimension_value);
        delta = distance / current_dimension_value;
        // Cij = Xi,j + Δij * (m - Xi,j).
        candidates[id_candit].chromosome[j] = current_dimension_value + delta * (mean - current_dimension_value);
        if (random_double(0, 1) <= parameters.p_exploitation || !(candidates[id_candit].chromosome[j] <= 100 && candidates[id_candit].chromosome[j] >= -100))
            candidates[id_candit].chromosome[j] = random_double(-100, 100);
    }
}

void chose_new_point(individuo *individuos, int id_individuo, int d)
{
    double chosen_porcent = random_double(0, 1);
    double probability_porcent = 0.0;

    for (int y = 0; y < parameters.num_candidates; y++)
    {
        probability_porcent += sum_pheromone_individuo(pheromones_candidates, d, y) / sum_pheromone_individuos(pheromones_candidates, d, parameters.num_candidates);
        if (probability_porcent <= chosen_porcent)
        {
            saved_chosen(individuos, id_individuo, y, d);
            fitness(&individuos[id_individuo], d, parameters.function_number);

            break;
        }
    }
}

void select_next_position(double **pheromones, individuo *individuos, int d, individuo *best_individuo)
{
    DEBUG(printf("select_next_position\n");)
    double sum_dimensions[d], prob;
    double chosen_porcent, probability_porcent = 0.0;

    for (int i = 0; i < d; i++)
    {
        sum_dimensions[i] = sum_individuo_dimension(individuos, i);
    }
    // Para cada formiga gera N candidatos com base no feromonio
    for (int i = 0; i < parameters.num_ants; i++)
    {
        for (int k = 0; k < parameters.num_candidates; k++)
        {
            candiate_calculator(individuos, pheromones, &sum_dimensions[0], d, i, k);
            // candiate_calculator_crossover(individuos, d, i, k);
        }
        update_pheromones(pheromones_candidates, candidates, parameters.num_candidates, d, best_individuo);
        chosen_porcent = random_double(0, 1);
        chose_new_point(individuos, i, d);
    }
}

double abs_double(double x)
{
    if (x < 0.0)
    {
        return -x;
    }
    else
    {
        return x;
    }
}

double sigma_sums(individuo *individuos, int dimension, individuo *best_individuo)
{
    long double sum_one = 0.0, sum_two = 0.0;
    long double subtraction_fitness = 0.0;

    for (int i = 0; i < parameters.num_ants; i++)
    {
        subtraction_fitness = best_individuo->fitness - individuos[i].fitness;
        if (subtraction_fitness == 0)
            subtraction_fitness = 0.001;
        sum_one += pow(best_individuo->chromosome[dimension] - individuos[i].chromosome[dimension], 2) / subtraction_fitness;
        sum_two += 1 / subtraction_fitness;
    }

    sum_one = sqrt(abs_double(sum_one / sum_two));
    return sum_one;
}

void update_sigma(individuo *individuos, int d, individuo *best_individuo)
{
    for (int i = 0; i < d; i++)
    {
        sigma[i] = sigma_sums(individuos, i, best_individuo);
    }
}

void print_sigma()
{
    for (int i = 0; i < 10; i++)
    {
        printf("Sigma %d: %lf\n", i, sigma[i]);
    }
}

void best_individuo_check(individuo *individuos, individuo *best_individuo, double **pheromones, int d)
{
    int confirm = 0;
    for (int i = 0; i < parameters.num_ants; i++)
    {
        if (individuos[i].fitness <= best_individuo->fitness)
            confirm = 1;
    }
    if (!confirm)
    {
        for (int i = 0; i < d; i++)
        {
            individuos[0].chromosome[i] = best_individuo->chromosome[i];
        }
        individuos[0].fitness = best_individuo->fitness;
    }
}

void catch_best_individuo(populacao *population, individuo *best_individuot, double **pheromones, int d)
{
    if (get_best_of_population(*population)->fitness < best_individuot->fitness)
    {
        copy_individuo(get_best_of_population(*population), best_individuot, parameters.dimension);
    }
}

double calc_mean_individuo(individuo *individuos, int individuos_size)
{
    double sum = 0.0;
    for (int i = 0; i < individuos_size; i++)
    {
        sum += individuos[i].fitness;
    }
    return sum / individuos_size;
}

double desvio_padrao_individuo(individuo *individuos, int individuos_size)
{
    double mean = calc_mean_individuo(individuos, individuos_size);
    double sum = 0.0;
    for (int i = 0; i < individuos_size; i++)
    {
        sum += pow(individuos[i].fitness - mean, 2);
    }
    return sqrt(sum / individuos_size);
}

// The main ACO function
populacao *aco(populacao *population)
{
    set_default_parameters_ant();
    // print_parameters(parameters);
    if (population == NULL)
    {
        population = generate_island(1, parameters.num_ants, parameters.dimension, parameters.domain_function, parameters.function_number);
    }

    individuo *individuos = population->individuos;
    int d = 10;
    DEBUG(printf("aco\n");)
    // Allocate memory for the pheromone matrix
    double **pheromones = (double **)malloc(parameters.num_ants * sizeof(double *));
    pheromones_candidates = (double **)malloc(parameters.num_candidates * sizeof(double *));
    time_t time_init, time_now;
    time(&time_init);
    time(&time_now);

    for (int i = 0; i < parameters.num_ants; i++)
    {
        pheromones[i] = (double *)malloc(d * sizeof(double));
        for (int j = 0; j < d; j++)
        {
            pheromones[i][j] = 1.0 / parameters.num_ants;
        }
    }

    for (int i = 0; i < parameters.num_candidates; i++)
    {
        pheromones_candidates[i] = (double *)malloc(d * sizeof(double));
        for (int j = 0; j < d; j++)
        {
            pheromones_candidates[i][j] = 1.0 / parameters.num_candidates;
        }
    }

    // Allocate memory for the individuos
    // individuo *individuos = (individuo *)malloc(parameters.num_ants * sizeof(individuo));
    individuo *best_individuo = generate_population(1, 10, parameters.domain_function, 15);
    candidates = (individuo *)malloc(parameters.num_candidates * sizeof(individuo));

    initialize(candidates, parameters.num_candidates, d);

    // Find the best individuo and its fitness value
    copy_individuo(get_best_of_population(*population), best_individuo, parameters.dimension);

    sigma = (double *)malloc(d * sizeof(double));
    update_sigma(individuos, d, best_individuo);

    // Update the pheromone matrix with the best individuo's path
    update_pheromones(pheromones, individuos, parameters.num_ants, d, best_individuo);
    int generations_count = 0;
    while (generations_count < parameters.num_generations_per_epoca && difftime(time_now, time_init) < parameters.time_limit)
    {
        DEBUG(print_individuo(individuos, d, best_individuo););
        //  Move each individuo to a new individuo_chromossome
        select_next_position(pheromones, individuos, d, best_individuo);
        evaporate_pheromones(pheromones, d);

        // Update the best individuo and its fitness value
        catch_best_individuo(population, best_individuo, pheromones, d);
        // Verifica se a melhor posição não foi perdida
        best_individuo_check(individuos, best_individuo, pheromones, d);
        // Update the pheromone matrix with the best individuo's path
        update_pheromones(pheromones, individuos, parameters.num_ants, d, best_individuo);
        update_sigma(individuos, d, best_individuo);
        time(&time_now);
        generations_count++;
    }

    population->individuos = individuos;
    population->size = parameters.num_ants;
    copy_individuo(best_individuo, &population->individuos[0], d);

    // Free memory
    // for (int i = 0; i < parameters.num_ants; i++)
    //{
    // free(pheromones[i]);
    // free(individuos[i].chromosome);
    // }

    // for (int i = 0; i < parameters.num_candidates; i++)
    //{
    // free(pheromones_candidates[i]);
    // free(candidates[i].chromosome);
    //}
    free(pheromones);
    free(pheromones_candidates);
    // free(candidates);
    free(sigma);
    free(best_individuo);
    reset_parameters_ant();
    return population;
}