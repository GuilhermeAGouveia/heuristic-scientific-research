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
double **pheromones_best;

typedef struct ant
{
    double *ant_chromossome;
    double fitness;
} Ant;
Ant *candidates;

typedef struct ant_parameters
{
    int num_ant;
    int num_iter;
    double tax_evaporate;
    int num_candidates;
    double p_exploitation;
    int function_number;
    int time_limit;
    int seed;
} AntParameters;

double abs_double(double x);
double f(double *x, int n);
void initialize(Ant *ants, int n, int d);
void update_pheromones(double **pheromones, Ant *ants, int n, int d);
void select_next_position(double **pheromones, Ant *ants, int d);
void aco(int d);

AntParameters parameters;

void set_default_parameters()
{
    parameters.num_ant = 200;
    parameters.num_iter = 2000;
    parameters.tax_evaporate = 0.1;
    parameters.num_candidates = 10;
    parameters.p_exploitation = 0.1;
    parameters.function_number = 1;
    parameters.time_limit = 10; // in seconds
    parameters.seed = time(NULL);
}

void print_usage()
{
    printf("Usage: ./ant -f <function_number> -t <time_limit> -i <num_iter> -p <p_exploitation> -e <tax_evaporate> -c <num_candidates> -a <num_ant>\n");
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
            parameters.num_ant = atoi(optarg);
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

int main(int argc, char *argv[])
{
    set_parameters(argc, argv);
    srand(parameters.seed);
    int d = 10;
    aco(d);
    return 0;
}

void print_ant(Ant *ant, int dimension, int id)
{

    for (int j = 0; j < dimension; j++)
    {
        printf("%lf ", ant[id].ant_chromossome[j]);
    }
    printf("-> %lf\n", ant[id].fitness);
}

void print_ants(Ant *ant, int dimension)
{
    for (int i = 0; i < parameters.num_ant; i++)
    {
        print_ant(ant, dimension, i);
    }
}

// Objective function to be optimized
double f(double *x, int n)
{
    double y;
    cec15_test_func(x, &y, n, 1, parameters.function_number);
    return y;
}

// Initialize the ants' positions randomly
void initialize(Ant *ants, int n, int d)
{
    DEBUG(printf("Initializing the ants\n");)
    for (int i = 0; i < n; i++)
    {
        ants[i].ant_chromossome = (double *)malloc(d * sizeof(double));
        for (int j = 0; j < d; j++)
        {
            ants[i].ant_chromossome[j] = random_double(-100, 100);
        }
        ants[i].fitness = f(ants[i].ant_chromossome, d);
    }
}
void evaporate_pheromones(double **pheromones, int dimension)
{
    for (int i = 0; i < parameters.num_ant; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            pheromones[i][j] = pheromones[i][j] * (1 - parameters.tax_evaporate);
        }
    }
}

// Update the pheromone information in the positions visited by the ants
void update_pheromones(double **pheromones, Ant *ants, int n, int d)
{
    DEBUG(printf("update_pheromones\n");)

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < d; j++)
        {
            pheromones[i][j] = (1 / sqrt(2 * sigma[j] * PI)) * exp(pow((pheromones_best[0][j] - pheromones[i][j]), 2) / (-2 * pow(sigma[j], 2)));
            // printf("pheromoneo: %lf\n",pheromones[i][j] );
            if (j % 2 == 0)
                pheromones[i][j] += random_double(0, 0.3);
        }
        // printf("\n\n");
    }
}

double sum_ant_dimension(Ant *ants, int current_d)
{
    double sum = 0.0;
    for (int i = 0; i < parameters.num_ant; i++)
    {
        sum += ants[i].ant_chromossome[current_d];
    }
    return sum;
}

double sum_pheromone_dimension(double **pheromones, int d)
{
    double sum = 0.0;
    for (int i = 0; i < parameters.num_ant; i++)
    {
        sum += pheromones[i][d];
    }
    return sum;
}

double sum_pheromone_ant(double **pheromones, int d, int id)
{
    double sum = 0.0;
    for (int i = 0; i < d; i++)
    {
        sum += pheromones[id][i];
    }
    return sum;
}

double sum_pheromone_ants(double **pheromones, int d, int n_ants)
{
    double sum = 0.0;
    for (int j = 0; j < n_ants; j++)
    {
        for (int i = 0; i < d; i++)
        {
            sum += pheromones[j][i];
        }
    }
    return sum;
}

void select_next_position2(double **pheromones, Ant *ants, int d)
{
    DEBUG(printf("select_next_position\n");)
    double sum_pheromone[d], prob;

    for (int i = 0; i < d; i++)
    {
        sum_pheromone[i] = sum_pheromone_dimension(pheromones, i);
    }
    // printf("Select_Sum_Pheromone:%lf\n", sum_pheromone[1]);

    for (int i = 0; i < parameters.num_ant; i++)
    {
        for (int j = 0; j < d; j++)
        {
            prob = pheromones[i][j] / sum_pheromone[j];

            if (((double)rand() / RAND_MAX) < prob)
            {
                ants[i].ant_chromossome[j] += random_double(-5, 5);
            }
        }
        ants[i].fitness = f(ants[i].ant_chromossome, d);
    }
}

void saved_chosen(Ant *ants, int id_origin, int id_new, int d)
{
    for (int i = 0; i < d; i++)
    {
        ants[id_origin].ant_chromossome[i] = candidates[id_new].ant_chromossome[i];
    }
}

void candiate_calculator_crossover(Ant *ants, int d, int id_ant, int id_candit)
{
    double current_dimension_value;
    int ant2 = rand() % parameters.num_ant;

    while (ant2 == id_ant)
        ant2 = rand() % parameters.num_ant;

    for (int j = 0; j < d; j++)
    {
        current_dimension_value = ants[id_ant].ant_chromossome[j];

        candidates[id_candit].ant_chromossome[j] = (current_dimension_value + ants[ant2].ant_chromossome[j]) / 2;
        // printf("Original: %lf  ", current_dimension_value);
        // printf("candidato: %lf\n", candidates[id_candit].ant_chromossome[j]);
        if (random_double(0, 1) <= parameters.p_exploitation || !(candidates[id_candit].ant_chromossome[j] <= 100 && candidates[id_candit].ant_chromossome[j] >= -100))
            candidates[id_candit].ant_chromossome[j] = random_double(-100, 100);
    }
}

void candiate_calculator(Ant *ants, double **pheromones, double *sum_ant_dimension, int d, int id_ant, int id_candit)
{
    double distance, current_pheromone, current_dimension_value, delta, mean;
    for (int j = 0; j < d; j++)
    {
        current_pheromone = pheromones[id_ant][j];
        current_dimension_value = ants[id_ant].ant_chromossome[j];
        mean = (sum_ant_dimension[j] - current_dimension_value) / parameters.num_ant - 1;
        distance = abs_double(mean - current_dimension_value);
        delta = distance / current_dimension_value;
        // Cij = Xi,j + Δij * (m - Xi,j).
        candidates[id_candit].ant_chromossome[j] = current_dimension_value + delta * (mean - current_dimension_value);
        // printf("Original: %lf  ", current_dimension_value);
        // printf("candidato: %lf\n", candidates[id_candit].ant_chromossome[j]);
        if (random_double(0, 1) <= parameters.p_exploitation || !(candidates[id_candit].ant_chromossome[j] <= 100 && candidates[id_candit].ant_chromossome[j] >= -100))
            candidates[id_candit].ant_chromossome[j] = random_double(-100, 100);
    }
}

void chose_new_point(Ant *ants, int id_ant, int d)
{
    double chosen_porcent = random_double(0, 1);
    double probability_porcent = 0.0;

    for (int y = 0; y < parameters.num_candidates; y++)
    {
        probability_porcent += sum_pheromone_ant(pheromones_candidates, d, y) / sum_pheromone_ants(pheromones_candidates, d, parameters.num_candidates);
        if (probability_porcent <= chosen_porcent)
        {
            saved_chosen(ants, id_ant, y, d);
            ants[id_ant].fitness = f(ants[id_ant].ant_chromossome, d);
            break;
        }
    }
}

void select_next_position(double **pheromones, Ant *ants, int d)
{
    DEBUG(printf("select_next_position\n");)
    double sum_dimensions[d], prob;
    double chosen_porcent, probability_porcent = 0.0;

    for (int i = 0; i < d; i++)
    {
        sum_dimensions[i] = sum_ant_dimension(ants, i);
    }
    // Para cada formiga gera N candidatos com base no feromonio
    for (int i = 0; i < parameters.num_ant; i++)
    {
        for (int k = 0; k < parameters.num_candidates; k++)
        {
            candiate_calculator(ants, pheromones, &sum_dimensions[0], d, i, k);
            // candiate_calculator_crossover(ants, d, i, k);
        }
        update_pheromones(pheromones_candidates, candidates, parameters.num_candidates, d);
        chosen_porcent = random_double(0, 1);
        chose_new_point(ants, i, d);
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

double sigma_sums(Ant *ants, int dimension, Ant *best_ant)
{
    long double sum_one = 0.0, sum_two = 0.0;
    long double subtraction_fitness = 0.0;

    for (int i = 0; i < parameters.num_ant; i++)
    {
        subtraction_fitness = best_ant->fitness - ants[i].fitness;
        if (subtraction_fitness == 0)
            subtraction_fitness = 0.001;
        sum_one += pow(best_ant->ant_chromossome[dimension] - ants[i].ant_chromossome[dimension], 2) / subtraction_fitness;
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

void update_sigma(Ant *ants, int d, Ant *best_ant)
{
    for (int i = 0; i < d; i++)
    {
        sigma[i] = sigma_sums(ants, i, best_ant);
    }
}

void print_sigma()
{
    for (int i = 0; i < 10; i++)
    {
        printf("Sigma %d: %lf\n", i, sigma[i]);
    }
}

void best_ant_check(Ant *ants, Ant *best_ant, double **pheromones, int d)
{
    int confirm = 0;
    for (int i = 0; i < parameters.num_ant; i++)
    {
        if (ants[i].fitness <= best_ant->fitness)
            confirm = 1;
    }
    if (!confirm)
    {
        for (int i = 0; i < d; i++)
        {
            ants[0].ant_chromossome[i] = best_ant->ant_chromossome[i];
            pheromones[0][i] = pheromones_best[0][i];
        }
        ants[0].fitness = best_ant->fitness;
    }
}

void catch_best_ant(Ant *ants, Ant *best_antt, double **pheromones, int d)
{
    double best_fitness = best_antt->fitness;
    int best_ant = -1;

    for (int i = 1; i < parameters.num_ant; i++)
    {
        if (ants[i].fitness < best_fitness)
        {
            best_fitness = ants[i].fitness;
            best_ant = i;
        }
    }

    if (best_ant != -1)
    {

        for (int i = 0; i < d; i++)
        {
            best_antt->ant_chromossome[i] = ants[best_ant].ant_chromossome[i];
            pheromones_best[0][i] = pheromones[best_ant][i];
        }
        best_antt->fitness = ants[best_ant].fitness;
    }
}

double calc_mean_ant(Ant *ants, int ants_size)
{
    double sum = 0.0;
    for (int i = 0; i < ants_size; i++)
    {
        sum += ants[i].fitness;
    }
    return sum / ants_size;
}

double desvio_padrao_ant(Ant *ants, int ants_size)
{
    double mean = calc_mean_ant(ants, ants_size);
    double sum = 0.0;
    for (int i = 0; i < ants_size; i++)
    {
        sum += pow(ants[i].fitness - mean, 2);
    }
    return sqrt(sum / ants_size);
}

// The main ACO function
void aco(int d)
{
    DEBUG(printf("aco\n");)
    // Allocate memory for the pheromone matrix
    double **pheromones = (double **)malloc(parameters.num_ant * sizeof(double *));
    pheromones_candidates = (double **)malloc(parameters.num_candidates * sizeof(double *));
    pheromones_best = (double **)malloc(1 * sizeof(double *));

    for (int i = 0; i < parameters.num_ant; i++)
    {
        pheromones[i] = (double *)malloc(d * sizeof(double));
        for (int j = 0; j < d; j++)
        {
            pheromones[i][j] = 1.0 / parameters.num_ant;
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

    pheromones_best[0] = (double *)malloc(d * sizeof(double));
    for (int j = 0; j < d; j++)
    {
        pheromones_best[0][j] = 1.0 / parameters.num_candidates;
    }

    // Allocate memory for the ants
    Ant *ants = (Ant *)malloc(parameters.num_ant * sizeof(Ant));
    Ant *best_ant = (Ant *)malloc(1 * sizeof(Ant));
    candidates = (Ant *)malloc(parameters.num_candidates * sizeof(Ant));

    // Initialize the ants' positions and fitness values
    initialize(ants, parameters.num_ant, d);
    initialize(best_ant, 1, d);
    initialize(candidates, parameters.num_candidates, d);

    // Find the best ant and its fitness value
    catch_best_ant(ants, best_ant, pheromones, d);

    sigma = (double *)malloc(d * sizeof(double));
    update_sigma(ants, d, best_ant);

    // Update the pheromone matrix with the best ant's path
    update_pheromones(pheromones, ants, parameters.num_ant, d);
    // Iterate over the specified number of iterations
    int max_inter_add = 300;
    int max_inter = max_inter_add;
    int cont_or_stop = 1;
    while (cont_or_stop)
    {
        double best_anter = best_ant->fitness;
        for (int iter = 0; iter < max_inter; iter++)
        {
            DEBUG(print_ant(ants, d, best_ant););
            printf("Best_fitness: %lf\n", best_ant->fitness);
            // Move each ant to a new ant_chromossome
            select_next_position(pheromones, ants, d);
            evaporate_pheromones(pheromones, d);

            // Update the best ant and its fitness value
            catch_best_ant(ants, best_ant, pheromones, d);
            // Verifica se a melhor posição não foi perdida
            best_ant_check(ants, best_ant, pheromones, d);
            // Update the pheromone matrix with the best ant's path
            update_pheromones(pheromones, ants, parameters.num_ant, d);
            update_sigma(ants, d, best_ant);
        }

        //double desv = desvio_padrao_ant(ants, parameters.num_ant);
        //printf("Desvio: %lf\n", desv);

        if (doubleEqual(best_anter, best_ant->fitness, 4))
            cont_or_stop = 0;
    }

    // Print the best solution found
    // print_ants(ants, d);
    printf("Best fitness value: %lf\n", best_ant->fitness);
    printf("Best ant_chromossome:");
    for (int i = 0; i < d; i++)
    {
        printf(" %lf", best_ant->ant_chromossome[i]);
    }
    printf("\n");
    printf("Best %lf\n", best_ant->fitness);

    // Free memory
    for (int i = 0; i < parameters.num_ant; i++)
    {
        free(pheromones[i]);
        free(ants[i].ant_chromossome);
    }

    for (int i = 0; i < parameters.num_candidates; i++)
    {
        free(pheromones_candidates[i]);
        free(candidates[i].ant_chromossome);
    }
    free(pheromones);
    free(ants);
    free(pheromones_candidates);
    free(candidates);
    free(sigma);
    free(best_ant);
    free(pheromones_best[0]);
}