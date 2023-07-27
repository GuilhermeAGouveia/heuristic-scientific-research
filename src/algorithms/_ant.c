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

#define DEBUG(x)

double *sigma;
double **pheromones_candidates;

individuo *candidates;

typedef struct individuo_parameters
{
    int num_individuo;
    int num_iter;
    double tax_evaporate;
    int num_candidates;
    double p_exploitation;
    int function_number;
    int time_limit;
    int seed;
} individuoParameters;

double abs_double(double x);
double f(double *x, int n);
void initialize(individuo *individuos, int n, int d);
void update_pheromones(double **pheromones, individuo *individuos, int n, int d, individuo *best_individuo);
void select_next_position(double **pheromones, individuo *individuos, int d, individuo *best_individuo);
populacao *aco();

individuoParameters parameters;

void set_default_parameters()
{
    parameters.num_individuo = 5252;
    parameters.num_iter = 2000;
    parameters.tax_evaporate = 0.31458;
    parameters.num_candidates = 9;
    parameters.p_exploitation = 0.25;
    parameters.function_number = 5;
    parameters.time_limit = 10; // in seconds
    parameters.seed = time(NULL);
}

void print_usage()
{
    printf("Usage: ./individuo -f <function_number> -t <time_limit> -i <num_iter> -p <p_exploitation> -e <tax_evaporate> -c <num_candidates> -a <num_individuo>\n");
}

void set_parameters(int argc, char *argv[])
{
    int opt;
    set_default_parameters();
    while ((opt = getopt(argc, argv, "f:t:i:p:e:c:a:s:")) != -1)
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
            parameters.num_iter = atoi(optarg);
            break;
        case 'p':
            parameters.p_exploitation = atof(optarg);
            break;
        case 'e':
            parameters.tax_evaporate = atof(optarg);
            break;
        case 'c':
            parameters.num_candidates = atoi(optarg);
            break;
        case 'a':
            parameters.num_individuo = atoi(optarg);
            break;
        case 's':
            parameters.seed = atoi(optarg);
            if (parameters.seed == 0)
                parameters.seed = time(NULL);
            break;
        default:
            printf("Invalid option: %c\n", opt);
            print_usage();
            exit(1);
            break;
        }
    }
}


void print_individuos(individuo *individuo, int dimension)
{
    for (int i = 0; i < parameters.num_individuo; i++)
    {
        print_individuo(*individuo, dimension, i);
    }
}

// Objective function to be optimized
double f(double *x, int n)
{
    double y;
    cec15_test_func(x, &y, n, 1, parameters.function_number);
    return y;
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
        individuos[i].fitness = f(individuos[i].chromosome, d);
    }
}
void evaporate_pheromones(double **pheromones, int dimension)
{
    for (int i = 0; i < parameters.num_individuo; i++)
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
            // printf("pheromoneo: %lf\n",pheromones[i][j] );
            if (j % 2 == 0)
                pheromones[i][j] += random_double(0, 0.3);
        }
        // printf("\n\n");
    }
}

double sum_individuo_dimension(individuo *individuos, int current_d)
{
    double sum = 0.0;
    for (int i = 0; i < parameters.num_individuo; i++)
    {
        sum += individuos[i].chromosome[current_d];
    }
    return sum;
}

double sum_pheromone_dimension(double **pheromones, int d)
{
    double sum = 0.0;
    for (int i = 0; i < parameters.num_individuo; i++)
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
    // printf("Select_Sum_Pheromone:%lf\n", sum_pheromone[1]);

    for (int i = 0; i < parameters.num_individuo; i++)
    {
        for (int j = 0; j < d; j++)
        {
            prob = pheromones[i][j] / sum_pheromone[j];

            if (((double)rand() / RAND_MAX) < prob)
            {
                individuos[i].chromosome[j] += random_double(-5, 5);
            }
        }
        individuos[i].fitness = f(individuos[i].chromosome, d);
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
    int individuo2 = rand() % parameters.num_individuo;

    while (individuo2 == id_individuo)
        individuo2 = rand() % parameters.num_individuo;

    for (int j = 0; j < d; j++)
    {
        current_dimension_value = individuos[id_individuo].chromosome[j];

        candidates[id_candit].chromosome[j] = (current_dimension_value + individuos[individuo2].chromosome[j]) / 2;
        // printf("Original: %lf  ", current_dimension_value);
        // printf("candidato: %lf\n", candidates[id_candit].individuo_chromossome[j]);
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
        mean = (sum_individuo_dimension[j] - current_dimension_value) / parameters.num_individuo - 1;
        distance = abs_double(mean - current_dimension_value);
        delta = distance / current_dimension_value;
        // Cij = Xi,j + Δij * (m - Xi,j).
        candidates[id_candit].chromosome[j] = current_dimension_value + delta * (mean - current_dimension_value);
        // printf("Original: %lf  ", current_dimension_value);
        // printf("candidato: %lf\n", candidates[id_candit].individuo_chromossome[j]);
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
            individuos[id_individuo].fitness = f(individuos[id_individuo].chromosome, d);
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
    for (int i = 0; i < parameters.num_individuo; i++)
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

    for (int i = 0; i < parameters.num_individuo; i++)
    {
        subtraction_fitness = best_individuo->fitness - individuos[i].fitness;
        if (subtraction_fitness == 0)
            subtraction_fitness = 0.001;
        sum_one += pow(best_individuo->chromosome[dimension] - individuos[i].chromosome[dimension], 2) / subtraction_fitness;
        sum_two += 1 / subtraction_fitness;
        // printf("sum_one-:%Lf\n", sum_one);
        // printf("sum_two-:%Lf\n", sum_two);
        // printf("subtract-:%Lf\n", subtraction_fitness);
    }
    // printf("sum_one-FINAL:%Lf\n", sum_one);
    // printf("sum_two-FINAL:%Lf\n", sum_two);
    sum_one = sqrt(abs_double(sum_one / sum_two));
    // printf("sigma-:%Lf\n", sum_one);
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
    for (int i = 0; i < parameters.num_individuo; i++)
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

void catch_best_individuo(individuo *individuos, individuo *best_individuot, double **pheromones, int d)
{
    double best_fitness = best_individuot->fitness;
    int best_individuo = -1;

    for (int i = 1; i < parameters.num_individuo; i++)
    {
        if (individuos[i].fitness < best_fitness)
        {
            best_fitness = individuos[i].fitness;
            best_individuo = i;
        }
    }

    if (best_individuo != -1)
    {

        for (int i = 0; i < d; i++)
        {
            best_individuot->chromosome[i] = individuos[best_individuo].chromosome[i];
        }
        best_individuot->fitness = individuos[best_individuo].fitness;
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
populacao *aco()
{
    int d = 10;
    DEBUG(printf("aco\n");)
    // Allocate memory for the pheromone matrix
    double **pheromones = (double **)malloc(parameters.num_individuo * sizeof(double *));
    pheromones_candidates = (double **)malloc(parameters.num_candidates * sizeof(double *));
    time_t time_init, time_now;
    time(&time_init);
    time(&time_now);

    for (int i = 0; i < parameters.num_individuo; i++)
    {
        pheromones[i] = (double *)malloc(d * sizeof(double));
        for (int j = 0; j < d; j++)
        {
            pheromones[i][j] = 1.0 / parameters.num_individuo;
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
    individuo *individuos = (individuo *)malloc(parameters.num_individuo * sizeof(individuo));
    individuo *best_individuo = (individuo *)malloc(1 * sizeof(individuo));
    candidates = (individuo *)malloc(parameters.num_candidates * sizeof(individuo));

    // Initialize the individuos' positions and fitness values
    initialize(individuos, parameters.num_individuo, d);
    initialize(best_individuo, 1, d);
    initialize(candidates, parameters.num_candidates, d);

    // Find the best individuo and its fitness value
    catch_best_individuo(individuos, best_individuo, pheromones, d);

    sigma = (double *)malloc(d * sizeof(double));
    update_sigma(individuos, d, best_individuo);

    // Update the pheromone matrix with the best individuo's path
    update_pheromones(pheromones, individuos, parameters.num_individuo, d, best_individuo);
    // Iterate over the specified number of iterations
    int max_inter_add = 100;
    int max_inter = 150;
    int cont_or_stop = 1;
    while (cont_or_stop && difftime(time_now, time_init) < parameters.time_limit)
    {
        double best_individuoer = best_individuo->fitness;
        for (int iter = 0; iter < max_inter && difftime(time_now, time_init) < parameters.time_limit; iter++)
        {
            DEBUG(print_individuo(individuos, d, best_individuo););
            // printf("Best_fitness: %lf\n", best_individuo->fitness);
            //  Move each individuo to a new individuo_chromossome
            select_next_position(pheromones, individuos, d, best_individuo);
            evaporate_pheromones(pheromones, d);

            // Update the best individuo and its fitness value
            catch_best_individuo(individuos, best_individuo, pheromones, d);
            // Verifica se a melhor posição não foi perdida
            best_individuo_check(individuos, best_individuo, pheromones, d);
            // Update the pheromone matrix with the best individuo's path
            update_pheromones(pheromones, individuos, parameters.num_individuo, d, best_individuo);
            update_sigma(individuos, d, best_individuo);
            time(&time_now);
        }

        // double desv = desvio_padrao_individuo(individuos, parameters.num_individuo);
        // printf("Desvio: %lf\n", desv);

        if (doubleEqual(best_individuoer, best_individuo->fitness, 2))
            cont_or_stop = 0;
    }

    populacao *population = malloc(sizeof(populacao));
    population->individuos = individuos;
    population->size = parameters.num_individuo;
    copy_individuo(&population->individuos[1],&population->individuos[0], d);

    // Print the best solution found
    // print_individuos(individuos, d);
    //printf("Best fitness value: %lf\n", best_individuo->fitness);
    //printf("Best individuo_chromossome:");
   // for (int i = 0; i < d; i++)
   // {
       // printf(" %lf", best_individuo->chromosome[i]);
   // }
    //printf("\n");
   // printf("Best %lf\n", best_individuo->fitness);

    // Free memory
    for (int i = 0; i < parameters.num_individuo; i++)
    {
        free(pheromones[i]);
        //free(individuos[i].chromosome);
    }

    for (int i = 0; i < parameters.num_candidates; i++)
    {
        free(pheromones_candidates[i]);
        free(candidates[i].chromosome);
    }
    free(pheromones);
    free(pheromones_candidates);
    free(candidates);
    free(sigma);
    free(best_individuo);
    return population;
}

int main(int argc, char *argv[])
{
    set_parameters(argc, argv);
    srand(parameters.seed);
    populacao *result;
    result = aco();
    printf("Best %lf\n", result->individuos[0].fitness);
    return 0;
}