#include "../libs/commom.h"

void set_default_parameters_diferencial()
{
    parameters.current_algorithm = DE;
    if (!parameters.F)
        parameters.F = 0.82787;
    if (!parameters.function_number)
        parameters.function_number = 3;
    if (!parameters.time_limit)
        parameters.time_limit = 10; // seconds
    if (!parameters.island_size)
        parameters.island_size = 1;
    // if (!parameters.population_size)
    //     parameters.population_size = 1665; //58;
    if (!parameters.dimension)
        parameters.dimension = 10; // 10 or 30
    if (!parameters.domain_function.min)
        parameters.domain_function.min = -100;
    if (!parameters.domain_function.max)
        parameters.domain_function.max = 100;
    if (!parameters.mutation_rate)
        parameters.mutation_rate = 5; // 4; // %
    if (!parameters.seed)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        unsigned long long millisecondsSinceEpoch =
            (unsigned long long)(tv.tv_sec) * 1000 +
            (unsigned long long)(tv.tv_usec) / 1000;
        parameters.seed = millisecondsSinceEpoch;
    }

    if (!parameters.evaluation_limit)
        parameters.evaluation_limit = 1490400;
    srand(parameters.seed);
}

void reset_parameters_diferencial()
{
    // reset_parameters("s:p:g:m:F:"); antigo
    reset_parameters("s:m:F:");
    DEBUG(print_parameters(parameters));
}

populacao *mutation_diferencial(populacao *populacao_original, int dimension, domain domain_function)
{
    DEBUG(printf("\nMutation\n"););

    // TODO: populacao_mutada[i] = populacao_original não ocorre em NAO_MUTAR
    populacao *populacao_mutada = generate_island(1, populacao_original->size, dimension, domain_function, parameters.function_number);

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
        fitness(&populacao_mutada->individuos[i], dimension, parameters.function_number);
        DEBUG(printf("Individuo %d\n", i); print_individuo(populacao_mutada->individuos[i], dimension, i);)
    }
    return populacao_mutada;
}

void selection_diferencial(populacao *population_original, populacao *population_mutation, int dimension)
{
    DEBUG(printf("\nselection_diferencial\n"););
    DEBUG(printf("População original\n"););
    DEBUG(print_population(population_original->individuos, population_original->size, dimension, 1););
    DEBUG(printf("População cruzamento\n"););
    DEBUG(print_population(population_mutation->individuos, population_mutation->size, dimension, 1););
    for (int i = 0; i < population_mutation->size; i++)
    {
        if (population_mutation->individuos[i].fitness < population_original->individuos[i].fitness)
            copy_individuo(&population_mutation->individuos[i], &population_original->individuos[i], parameters.dimension);
        // population_original->individuos[i] = population_mutation->individuos[i];
    }
    qsort(population_original->individuos, population_original->size, sizeof(individuo), comparador_individuo);
    destroy_island(population_mutation, 1);
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

populacao *crossover_diferencial(populacao *populacao_original, populacao *populacao_mutada, int dimension)
{
    populacao *nova_populacao = generate_island(1, populacao_original->size, dimension, parameters.domain_function, parameters.function_number);
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
        fitness(&nova_populacao->individuos[i], dimension, parameters.function_number);
    }
    destroy_island(populacao_mutada, 1);
    return nova_populacao;
}

populacao *diferencial(populacao *population, int epoca_num, int current_generation, int population_num)
{
    set_default_parameters_diferencial();
    // print_parameters(parameters);
    if (population == NULL)
    {
        population = generate_island(1, parameters.population_size, parameters.dimension, parameters.domain_function, parameters.function_number);
    }

    DEBUG(printf("\nevolution\n"););
    individuo *parents[2];
    individuo bestIndividuo = {.fitness = INFINITY};

    int evaluation_count = 0;
    int epoca_count = 0;
    double best_ep_ant = bestIndividuo.fitness;
    DEBUG(printf("Iniciando evolucao\n"););

    populacao *original_population = population;
    populacao *cross_population;
    populacao *mutation_population;
    int generation_count = 0;

    while (generation_count < parameters.num_generations_per_epoca)
    {
        mutation_population = mutation_diferencial(original_population, parameters.dimension, parameters.domain_function);
        cross_population = crossover_diferencial(original_population, mutation_population, parameters.dimension);
        selection_diferencial(original_population, cross_population, parameters.dimension);

        // print_individuo(original_population->individuos[original_population->size - 1], dimension, 1);

        LOG(write_population_log(epoca_num, population_num, generation_count + current_generation, *original_population, parameters););
        STATISTICS(print_coords(&original_population->individuos[original_population->size - 1], 1, generation_count, parameters.num_generations_per_epoca););
        DEBUG(printf("\nGeração: %d\n", generation_count););
        generation_count++;
    }
    reset_parameters_diferencial();
    return original_population;
}
