#include "aco.h"
#include "../libs/commom.h"

double *sigma;
double **pheromones_candidates;
double **pheromones;
domain **probability_interval;
individuo *candidates, *best_individuo;

void set_default_parameters_ant()
{
    parameters.current_algorithm = ACO;
    // if (!parameters.population_size)
    //     parameters.population_size = 2828; // 5252;
    if (!parameters.tax_evaporate)
        parameters.tax_evaporate = 0.63855;
    if (!parameters.num_candidates)
        parameters.num_candidates = 20;
    if (!parameters.p_exploitation)
        parameters.p_exploitation = 0.00;
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
    // reset_parameters("s:e:n:P:g:p"); antigo
    reset_parameters("s:");
    DEBUG(print_parameters(parameters););
}

void print_individuos(individuo *individuo, int dimension)
{
    for (int i = 0; i < parameters.population_size; i++)
    {
        print_individuo(*individuo, dimension, i);
    }
}

// Update the pheromone information in the positions visited by the individuos
void update_pheromones(double **pheromones, individuo *individuos, int size, individuo *best_individuo)
{
    DEBUG(printf("update_pheromones\n");)
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < parameters.dimension; j++)
        {
            if (sigma[j] != 0)
            {
                pheromones[i][j] = (1 / sqrt(2 * sigma[j] * PI)) * exp(pow((best_individuo->chromosome[j] - individuos[i].chromosome[j]), 2) / (-2 * pow(sigma[j], 2)));
                if (j % 2 == 0)
                    pheromones[i][j] += random_double(0, 0.3);
            }
        }
    }
}

double sum_individuo_dimension(individuo *individuos, int current_d)
{
    double sum = 0.0;
    for (int i = 0; i < parameters.population_size; i++)
    {
        sum += individuos[i].chromosome[current_d];
    }
    return sum;
}

double sum_pheromone_dimension(double **pheromonesD, int d)
{
    double sum = 0.0;
    for (int i = 0; i < parameters.population_size; i++)
    {
        sum += pheromonesD[i][d];
    }
    return sum;
}

double sum_pheromone_individuo(double **pheromonesI, int id)
{
    double sum = 0.0;
    for (int i = 0; i < parameters.dimension; i++)
    {
        sum += pheromonesI[id][i];
    }
    return sum;
}

double sum_pheromone_individuos(double **pheromonesI, int n_individuos)
{
    double sum = 0.0;
    for (int j = 0; j < n_individuos; j++)
    {
        for (int i = 0; i < parameters.dimension; i++)
        {
            sum += pheromonesI[j][i];
        }
    }
    return sum;
}

void saved_chosen(individuo *individuos, int id_origin, int id_new)
{
    for (int i = 0; i < parameters.dimension; i++)
    {
        individuos[id_origin].chromosome[i] = candidates[id_new].chromosome[i];
    }
}

void normalize_probability()
{
    double sum_dimension, prob = 0;
    for (int i = 0; i < parameters.dimension; i++)
    {
        double start = 0;
        sum_dimension = sum_pheromone_dimension(pheromones, i);
        for (int j = 0; j < parameters.population_size; j++)
        {
            double pheromone = pheromones[j][i];
            if (sum_dimension != 0.0)
            {
                prob = pheromone / sum_dimension;
                probability_interval[j][i].min = start;
                probability_interval[j][i].max = start + prob;
                start += prob;
            }
            else
            {
                probability_interval[j][i].min = 0.0;
                probability_interval[j][i].max = 0.0;
            }
        }
    }
}

void candiate_calculator(individuo *individuos, double *sum_individuo_dimension, int id_candit)
{
    double chosen_porcent;
    for (int i = 0; i < parameters.dimension; i++)
    {
        chosen_porcent = random_double(0, 1);
        for (int j = 0; j < parameters.population_size; j++)
        {
            if (inInterval(probability_interval[j][i].min, probability_interval[j][i].max, chosen_porcent))
            {
                candidates[id_candit].chromosome[i] = individuos[j].chromosome[i];
                if (random_double(0, 1) <= parameters.p_exploitation)
                    candidates[id_candit].chromosome[i] = random_double(-100, 100);
                break;
            }
        }
    }
}

void chose_new_point(individuo *individuos, int id_individuo)
{
    double chosen_porcent = random_double(0, 1);
    double probability_porcent = 0.0;

    for (int y = 0; y < parameters.num_candidates; y++)
    {
        probability_porcent += sum_pheromone_individuo(pheromones_candidates, y) / sum_pheromone_individuos(pheromones_candidates, parameters.num_candidates);
        if (probability_porcent <= chosen_porcent)
        {
            saved_chosen(individuos, id_individuo, y);
            fitness(&individuos[id_individuo], parameters.dimension, parameters.function_number);

            break;
        }
    }
}

void select_next_position(individuo *individuos, individuo *best_individuo)
{
    DEBUG(printf("select_next_position\n");)
    double sum_dimensions[parameters.dimension], prob;
    double chosen_porcent, probability_porcent = 0.0;

    for (int i = 0; i < parameters.dimension; i++)
    {
        sum_dimensions[i] = sum_individuo_dimension(individuos, i);
    }
    // Para cada formiga gera N candidatos com base no feromonio
    normalize_probability();
    for (int i = 0; i < parameters.population_size; i++)
    {
        for (int k = 0; k < parameters.num_candidates; k++)
        {
            candiate_calculator(individuos, &sum_dimensions[0], k);
            // candiate_calculator_crossover(individuos, d, i, k);
        }
        update_pheromones(pheromones_candidates, candidates, parameters.num_candidates, best_individuo);
        chosen_porcent = random_double(0, 1);
        chose_new_point(individuos, i);
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

    for (int i = 0; i < parameters.population_size; i++)
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

void best_individuo_check(individuo *individuos, individuo *best_individuo)
{
    int confirm = 0;
    for (int i = 0; i < parameters.population_size; i++)
    {
        if (individuos[i].fitness <= best_individuo->fitness)
            confirm = 1;
    }
    if (!confirm)
    {
        for (int i = 0; i < parameters.dimension; i++)
        {
            individuos[0].chromosome[i] = best_individuo->chromosome[i];
        }
        individuos[0].fitness = best_individuo->fitness;
    }
}

void catch_best_individuo(populacao *population, individuo *best_individuot)
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

void alloc_memory()
{
    pheromones = (double **)malloc(parameters.population_size * sizeof(double *));
    pheromones_candidates = (double **)malloc(parameters.num_candidates * sizeof(double *));
    probability_interval = (domain **)malloc(parameters.population_size * sizeof(domain *));
    sigma = (double *)malloc(parameters.dimension * sizeof(double));
    best_individuo = generate_population(1, parameters.dimension, parameters.domain_function, parameters.function_number);
    candidates = generate_population(parameters.num_candidates, parameters.dimension, parameters.domain_function, parameters.function_number);

    for (int i = 0; i < parameters.population_size; i++)
    {
        pheromones[i] = (double *)malloc(parameters.dimension * sizeof(double));
        probability_interval[i] = (domain *)malloc(parameters.dimension * sizeof(domain));
        for (int j = 0; j < parameters.dimension; j++)
        {
            pheromones[i][j] = 1.0 / parameters.population_size;
            probability_interval[i][j].max = 0.0;
            probability_interval[i][j].min = 0.0;
        }
    }

    for (int i = 0; i < parameters.num_candidates; i++)
    {
        pheromones_candidates[i] = (double *)malloc(parameters.dimension * sizeof(double));
        for (int j = 0; j < parameters.dimension; j++)
        {
            pheromones_candidates[i][j] = 1.0 / parameters.num_candidates;
        }
    }
}

void desaloc_memory_aco()
{

    destroy_matriz(pheromones, parameters.population_size);
    destroy_matriz(pheromones_candidates, parameters.num_candidates);
    for (int i = 0; i < parameters.population_size; i++)
    {
        free(probability_interval[i]);
    }
    destroy_population(candidates, parameters.num_candidates);
    destroy_population(best_individuo, 1);
    free(probability_interval);
    free(sigma);
}

// The main ACO function
populacao *aco(populacao *population, int epoca_num, int current_generation, int population_num)
{
    set_default_parameters_ant();
    // print_parameters(parameters);
    if (population == NULL)
    {
        population = generate_island(1, parameters.population_size, parameters.dimension, parameters.domain_function, parameters.function_number);
        alloc_memory();
    }

    individuo *individuos = population->individuos;
    DEBUG(printf("aco\n");)

    // Find the best individuo and its fitness value
    copy_individuo(get_best_of_population(*population), best_individuo, parameters.dimension);
    update_sigma(individuos, parameters.dimension, best_individuo);

    // Update the pheromone matrix with the best individuo's path
    update_pheromones(pheromones, individuos, parameters.population_size, best_individuo);
    int generations_count = 0;
    while (generations_count < parameters.num_generations_per_epoca)
    {
        DEBUG(print_individuo(*individuos, parameters.dimension, best_individuo););
        select_next_position(individuos, best_individuo);
        catch_best_individuo(population, best_individuo);
        // Verifica se a melhor posição não foi perdida
        best_individuo_check(individuos, best_individuo);
        update_pheromones(pheromones, individuos, parameters.population_size, best_individuo);
        update_sigma(individuos, parameters.dimension, best_individuo);
        LOG(write_population_log(epoca_num, population_num, generations_count + current_generation, *population, parameters););
        generations_count++;
    }

    population->individuos = individuos;
    population->size = parameters.population_size;
    copy_individuo(best_individuo, &population->individuos[0], parameters.dimension);
    reset_parameters_ant();
    return population;
}