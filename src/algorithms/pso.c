#include "../libs/commom.h"
/*5.505.098*/
void set_default_parameters_pso()
{
    parameters.current_algorithm = PSO;
    if (!parameters.function_number)
        parameters.function_number = 3;
    if (!parameters.time_limit)
        parameters.time_limit = 10; // seconds
    if (!parameters.population_size)
        parameters.population_size = 9111;
    if (!parameters.dimension)
        parameters.dimension = 10; // 10 or 30
    if (!parameters.domain_function.min)
        parameters.domain_function.min = -100;
    if (!parameters.domain_function.max)
        parameters.domain_function.max = 100;
    if (!parameters.num_generations_per_epoca)
        parameters.num_generations_per_epoca = (int)(5505098/9111);//153;
    if (!parameters.seed)
        parameters.seed = time(NULL);
    if (!parameters.c1)
        parameters.c1 = 0.61845;
    if (!parameters.c2)
        parameters.c2 = 0.74494;
    srand(parameters.seed);
}

void reset_parameters_pso()
{
    parameters.population_size = 0;
    parameters.num_generations_per_epoca = 0;
    parameters.c1 = 0;
    parameters.c2 = 0;
    parameters.seed = 0;
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

void copy_individuo_pso(individuo *original, individuo *copia, int dimension)
{
    copia->fitness = original->fitness;
    for (int i = 0; i < dimension; i++)
    {
        copia->chromosome[i] = original->chromosome[i];
        copia->velocidade[i] = original->velocidade[i];
    }
}

populacao *pso(populacao *population, int epoca_num, int population_num)
{
    set_default_parameters_pso();
    // print_parameters(parameters);
    // print_individuo(population->individuos[2], parameters.dimension, 0);
    if (population == NULL)
    {
        population = generate_island(1, parameters.population_size, parameters.dimension, parameters.domain_function, parameters.function_number);
    }
    populacao *population_best_current;
    population_best_current = generate_island(1, parameters.population_size, parameters.dimension, parameters.domain_function, parameters.function_number);

    time_t time_init, time_now;
    time(&time_init);
    time(&time_now);

    individuo *individuo_best = generate_population(1, parameters.dimension, parameters.domain_function, parameters.function_number);
    double w_max = 1, w_min = 0;
    double *c1 = malloc(parameters.dimension * sizeof(double));
    double *c2 = malloc(parameters.dimension * sizeof(double));
    int generation_count = 0;

    while (generation_count < parameters.num_generations_per_epoca && difftime(time_now, time_init) < parameters.time_limit)
    {
        for (int i = 0; i < parameters.population_size; i++)
        {

            if (population->individuos[i].fitness < population_best_current->individuos[i].fitness)
                copy_individuo_pso(&population->individuos[i], &population_best_current->individuos[i], parameters.dimension);
            if (population->individuos[i].fitness < individuo_best->fitness)
                copy_individuo_pso(&population->individuos[i], individuo_best, parameters.dimension);

            double r1 = (double)rand() / RAND_MAX;
            double r2 = (double)rand() / RAND_MAX;
            // calcula_componente(c1, &population_best_current->individuos[i], &population->individuos[i], parameters.dimension);
            // calcula_componente(c2, individuo_best, &population->individuos[i], parameters.dimension);

            // w(t) = w_max - ((w_max - w_min) * t) / T
            double w = w_max - ((w_max - w_min) * generation_count) / parameters.num_generations_per_epoca;
            for (int j = 0; j < parameters.dimension; j++)
            {
                // v_{i,d}(t+1) = wv_{i,d}(t) + c1r1*(pbest_{i,d}-x_{i,d}(t)) + c2r2(gbest_{d}-x_{i,d}(t))
                population->individuos[i].velocidade[j] = population->individuos[i].velocidade[j] * w +
                                                          parameters.c1 * r1 * (population_best_current->individuos[i].chromosome[j] - population->individuos[i].chromosome[j]) +
                                                          parameters.c2 * r2 * (individuo_best->chromosome[j] - population->individuos[i].chromosome[j]);
            }

            atualiza_posicao(&population->individuos[i], parameters.dimension);
            fitness(&population->individuos[i], parameters.dimension, parameters.function_number);
        }
        LOG(write_population_log(epoca_num, population_num, generation_count, *population, parameters););

        time(&time_now);
        generation_count++;
    }
    copy_individuo_pso(individuo_best, &population->individuos[0], parameters.dimension);
    destroy_island(population_best_current, 1);
    reset_parameters_pso();
    return population;
}