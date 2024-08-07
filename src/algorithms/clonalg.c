#include "../libs/commom.h"

void set_default_parameters_clonalg()
{
    parameters.current_algorithm = CLONALG;
    if (!parameters.function_number)
        parameters.function_number = 3;
    if (!parameters.time_limit)
        parameters.time_limit = 10; // seconds
    // if (!parameters.population_size)
    //     parameters.population_size = 46;
    if (!parameters.dimension)
        parameters.dimension = 10; // 10 or 30
    if (!parameters.domain_function.min)
        parameters.domain_function.min = -100;
    if (!parameters.domain_function.max)
        parameters.domain_function.max = 100;
    if (!parameters.clones)
        parameters.clones = 72;
    if (!parameters.seed)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        unsigned long long millisecondsSinceEpoch =
            (unsigned long long)(tv.tv_sec) * 1000 +
            (unsigned long long)(tv.tv_usec) / 1000;
        parameters.seed = millisecondsSinceEpoch;
    }

    srand(parameters.seed);
}

void reset_parameters_clonalg()
{
    // reset_parameters("s:p:g:c:"); antigo
    reset_parameters("s:");
    DEBUG(print_parameters(parameters));
}

void mutation_n_genes(populacao *populacao_clones, int n_genes, int dimension, domain domain_function)
{
    DEBUG(printf("\nmutation\n"););
    if (n_genes > dimension)
    {
        printf("n_genes > dimension\n");
        exit(1);
    }

    int mutation_point;
    DEBUG(printf("Original:\n"););
    DEBUG(print_individuo(populacao_clones->individuos[0], dimension, 0););
    DEBUG(printf("Mutated %d genes:\n", n_genes););
    for (int i = 0; i < populacao_clones->size; i++)
    {
        int *mutation_positions = calloc(dimension, sizeof(int));
        for (int j = 0; j < n_genes; j++)
        {
            do
            {
                mutation_point = rand() % dimension;
            } while (mutation_positions[mutation_point]);
            mutation_positions[mutation_point] = 1;
            populacao_clones->individuos[i].chromosome[mutation_point] = random_double(domain_function.min, domain_function.max);
        }
        fitness(&populacao_clones->individuos[i], dimension, parameters.function_number);
        DEBUG(print_individuo(populacao_clones->individuos[i], dimension, i););
        free(mutation_positions);
    }
}

// log_scale: [c, d] -> [a, b]
// log_scale(c) = a
// log_scale(d) = b
double log_scale(double x, double a, double b, double c, double d)
{
    double result = a + (b - a) * (log(x) - log(c)) / (log(d) - log(c));
    return result;
}

populacao *generate_clones(populacao *population_main, int clone_number, int dimension, domain domain_function)
{
    DEBUG(printf("\ngenerate_clones\n"););
    populacao *populacao_clones = generate_island(population_main->size, clone_number, dimension, domain_function, parameters.function_number);
    DEBUG(printf("Original:\n"););
    DEBUG(print_population(population_main->individuos, population_main->size, dimension, 1););
    for (int i = 0; i < population_main->size; i++)
    {
        populacao_clones[i].n_mutations = ceil(log_scale(population_main->size - i, 1, dimension, 1, population_main->size));
        populacao_clones[i].size = clone_number;
        for (int j = 0; j < clone_number; j++)
        {
            copy_individuo(&population_main->individuos[i], &populacao_clones[i].individuos[j], dimension);
        }
        DEBUG(printf("Clone %d:\n", i););
        DEBUG(print_population(populacao_clones[i].individuos, clone_number, dimension, 1););
    }
    return populacao_clones;
}

populacao *mutation_clones(populacao *populacao_clones, int num_population_clones, int dimension, domain domain_function)
{
    DEBUG(printf("\nmutation_clones\n"););
    for (int i = 0; i < num_population_clones; i++)
    {
        DEBUG(printf("\nmutation_clones %d\n", i););
        mutation_n_genes(&populacao_clones[i], populacao_clones[i].n_mutations, dimension, domain_function);
        DEBUG(print_population(populacao_clones[i].individuos, populacao_clones[i].size, dimension, 1););
    }
    return populacao_clones;
}

void union_populacao_clones_and_main(populacao *populacao_clones, populacao *population_main, int num_population_clones)
{
    DEBUG(printf("\nunion_populacao_clones_and_main\n"););
    DEBUG(printf("Original:\n"););
    DEBUG(print_population(population_main->individuos, population_main->size, parameters.dimension, 1););
    for (int i = 0; i < num_population_clones; i++)
    {
        individuo *best_clone = get_best_of_population(populacao_clones[i]);
        if (best_clone->fitness < population_main->individuos[i].fitness)
        {
            copy_individuo(best_clone, &population_main->individuos[i], parameters.dimension);
        }
    }
}

populacao *clonalg(populacao *population, int epoca_num, int current_generation, int population_num)
{
    set_default_parameters_clonalg();
    // print_parameters(parameters);
    if (population == NULL)
    {
        population = generate_island(1, parameters.population_size, parameters.dimension, parameters.domain_function, parameters.function_number);
    }
    DEBUG(printf("\nevolution\n"););
    // populacao *population_main = generate_island(1, parameters.population_size, parameters.dimension, parameters.domain_function, parameters.function_number);
    populacao *population_main = population;
    qsort(population_main->individuos, population_main->size, sizeof(individuo), comparador_individuo);
    populacao *populacao_clones;
    populacao *populacao_clones_mutated;
    int generation_count = 0;
    DEBUG(printf("Iniciando evolucao\n"););
    //  while (difftime(time_now, time_init) < parameters.time_limit)
    // {

    while (generation_count < parameters.num_generations_per_epoca)
    {
        STATISTICS(print_coords(&population_main->individuos[population_main->size - 1], 1, generation_count, parameters.num_generations););

        DEBUG(printf("Populacao principal na geracao %d:\n", generation_count););
        DEBUG(print_population(population_main->individuos, population_main->size, parameters.dimension, 1););
        populacao_clones = generate_clones(population_main, parameters.clones, parameters.dimension, parameters.domain_function);
        populacao_clones_mutated = mutation_clones(populacao_clones, parameters.population_size, parameters.dimension, parameters.domain_function);
        union_populacao_clones_and_main(populacao_clones_mutated, population_main, parameters.population_size);
        destroy_island(populacao_clones, parameters.population_size);
        qsort(population_main->individuos, population_main->size, sizeof(individuo), comparador_individuo);
        LOG(write_population_log(epoca_num, population_num, generation_count + current_generation, *population_main, parameters););

        generation_count++;
    }
    // double desv = desvio_padrao(population_main->individuos, parameters.population_size);
    // printf("Desvio_P: %lf\n", desv);
    // printf("Anterior: %lf, Atual_best:%lf\n", best_anter, population_main->individuos[population_main->size - 1].fitness);

    reset_parameters_clonalg();
    return population_main;
}
