#include "commom.h"

void fitness(individuo *individuo, int dimension, int function_number)
{
    // individuo.fitness = real_function(individuo.chromosome, dimension);
    cec15_test_func(individuo->chromosome, &individuo->fitness, dimension, 1, function_number);
    // double x = individuo->chromosome[0];
    // double y = individuo->chromosome[1];
    // individuo->fitness = pow(x, 2) + pow(y, 2) - cos(18 * x) - cos(18 * y);
    // printf("fitness: %f\n", individuo->fitness);
}

int comparador_proporcoes_alg(const void *a, const void *b)
{
    proporcao_alg *v1 = (proporcao_alg *)a;
    proporcao_alg *v2 = (proporcao_alg *)b;

    return v1->proporcao < v2->proporcao;
}

int comparador_individuo(const void *a, const void *b)
{
    individuo *v1 = (individuo *)a;
    individuo *v2 = (individuo *)b;

    return v1->fitness < v2->fitness;
}

individuo *generate_population(int n_individuos, int dimension, domain domain_function, int function_number)
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
        fitness(&population[i], dimension, function_number);
    }
    return population;
}

populacao *generate_island(int island_size, int population_size, int dimension, domain domain_function, int function_number)
{
    DEBUG(printf("\ngenerate_island\n"););
    populacao *populations = malloc(island_size * sizeof(populacao));
    for (int i = 0; i < island_size; i++)
    {
        populations[i].individuos = generate_population(population_size, dimension, domain_function, function_number);
        populations[i].size = population_size;
        populations[i].crossover = rand() % 6;
        populations[i].neighbours = calloc(4, sizeof(populacao *));
        populations[i].neighbours[0] = &populations[(i + 1) % island_size]; // talvez isso dê problema
        populations[i].neighbours[1] = &populations[(i + 3) % island_size]; // talvez isso dê problema
    }
    return populations;
}

individuo *generate_clean_population(int n_individuos, int dimension)
{
    DEBUG(printf("\ngenerate_population\n"););
    individuo *population = malloc(n_individuos * sizeof(individuo));
    for (int i = 0; i < n_individuos; i++)
    {
        population[i].chromosome = (double *)malloc(dimension * sizeof(double));
        population[i].velocidade = (double *)malloc(dimension * sizeof(double));
        for (int j = 0; j < dimension; j++)
        {
            population[i].chromosome[j] = 0;
            population[i].velocidade[j] = 0;
        }
        population[i].fitness = 0;
    }
    return population;
}

populacao *generate_clean_island(int island_size, int population_size, int dimension)
{
    DEBUG(printf("\ngenerate_island\n"););
    populacao *populations = malloc(island_size * sizeof(populacao));
    for (int i = 0; i < island_size; i++)
    {
        populations[i].individuos = generate_clean_population(population_size, dimension);
        populations[i].size = population_size;
        populations[i].crossover = rand() % 6;
        populations[i].neighbours = calloc(4, sizeof(populacao *));
        populations[i].neighbours[0] = &populations[(i + 1) % island_size]; // talvez isso dê problema
        populations[i].neighbours[1] = &populations[(i + 3) % island_size]; // talvez isso dê problema
    }
    return populations;
}

individuo *get_best_of_population(populacao populacao)
{
    DEBUG(printf("\nget_best_of_population\n"););
    qsort(populacao.individuos, populacao.size, sizeof(individuo), comparador_individuo);
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
    }
}

void destroy_population(individuo *population, int n_individuos)
{
    DEBUG(printf("\ndestroy_population\n"););
    if (population != NULL)
    {
        for (int i = 0; i < n_individuos; i++)
        {
            free(population[i].chromosome);
            free(population[i].velocidade);
        }
    }
    free(population);
}

void destroy_island(populacao *populations, int island_size)
{
    DEBUG(printf("\ndestroy_island\n"););
    for (int i = 0; i < island_size; i++)
    {
        destroy_population(populations[i].individuos, populations[i].size);
        free(populations[i].neighbours);
    }
    // free(populations);
}

void migrate(populacao **populations, int island_size, int num_migrations, int dimension, domain domain_function, int function_number)
{
    DEBUG(printf("\nmigrate\n"););
    populacao *vizinho;
    // print_population(populations[0].individuos, populations[0].size, dimension, 1);

    for (int i = 0; i < island_size; i++)
    {
        DEBUG(printf("Populacao %d\n", i));
        populacao *current_island = populations[i];
        individuo *population = current_island->individuos;
        for (int k = 1; k <= num_migrations; k++)
        {
            individuo *melhor_individuo_da_populacao = &population[current_island->size - k];
            DEBUG(printf("Melhor individuo da populacao %d: %lf\n", i, melhor_individuo_da_populacao->fitness););
            for (int j = 0; j < 4; j++)
            {
                vizinho = current_island->neighbours[j];
                if (vizinho == NULL)
                    continue;
                individuo *neighbour_population = vizinho->individuos;
                individuo *pior_indivuduo_do_vizinho = get_worst_of_population(neighbour_population, vizinho->size);
                if (melhor_individuo_da_populacao->fitness > pior_indivuduo_do_vizinho->fitness)
                {
                    individuo *new_worst = generate_population(1, dimension, domain_function, function_number);
                    copy_individuo(new_worst, pior_indivuduo_do_vizinho, dimension);
                }
                else
                    copy_individuo(melhor_individuo_da_populacao, pior_indivuduo_do_vizinho, dimension);
            }
        }
    }
}

void swap_individuo(individuo *a, individuo *b)
{
    individuo temp = *a;
    *a = *b;
    *b = temp;
}

void shuffle(individuo *pool, int size_pool)
{
    for (int i = 0; i < size_pool; i++)
        swap_individuo(&pool[rand() % size_pool], &pool[rand() % size_pool]);
}

void random_random_migrate(populacao **populations, int island_size, int num_migrations, int dimension, domain domain_function, int function_number)
{
    DEBUG(printf("\nrandom random migrate\n"););
    individuo *pool = generate_population(num_migrations * island_size, dimension, domain_function, function_number);
    int positions[island_size][num_migrations];
    for (int i = 0; i < island_size; i++)
    {
        DEBUG(printf("população %d\n", i););
        for (int j = 0; j < num_migrations; j++)
        {
            int rand_index = rand() % populations[i]->size;
            DEBUG(printf("posição sorteada %d\n", rand_index););
            if (!populations[i]->individuos[rand_index].chromosome)
            { // se for nulo que dizer que o elemento já foi sorteado
                j--;
                DEBUG(printf("posição já foi sorteada\n"););
                continue;
            }

            pool[i * num_migrations + j] = populations[i]->individuos[rand_index]; // corre o risco de sortear o mesmo individuo
            populations[i]->individuos[rand_index] = (individuo){NULL, INFINITY, NULL};
            positions[i][j] = rand_index;
        }
    }

    shuffle(pool, num_migrations * island_size);

    for (int i = 0; i < island_size; i++)
    {
        for (int j = 0; j < num_migrations; j++)
        {
            populations[i]->individuos[positions[i][j]] = pool[i * num_migrations + j];
        }
    }
}

double euclidian(individuo firstIndividuo, individuo secondIndividuo, int dimension)
{
    double distance = 0;
    for (int i = 0; i < dimension; i++)
    {
        distance += pow(firstIndividuo.chromosome[i] - secondIndividuo.chromosome[i], 2);
    }
    distance = sqrt(distance);
    return distance;
}

double *densityPopulation(populacao **populations, int island_number)
{
    DEBUG(printf("\ndensityPopulation\n"););
    double average = 0;
    double sd = 0;
    double *sum = (double *)calloc(island_number, sizeof(double));
    double sumIndividual;
    double *result = malloc(2 * sizeof(double));
    // for all populations
    for (int i = 0; i < island_number; i++)
    {
        int nIndividuals = populations[i]->size;

        // for all individuals from population i
        for (int j = 0; j < nIndividuals - 1; j++)
        {
            // against all individuals from the same population
            for (int k = j + 1; k < nIndividuals; k++)
            {
                // sums with the norm-2 of individual j and k
                sum[i] += euclidian(populations[i]->individuos[j], populations[i]->individuos[k], 10);
            }
        }
    }

    for (int i = 0; i < island_number; i++)
    {
        average += sum[i];
        // DEBUG(printf("%lf"););
    }

    average /= island_number;

    for (int i = 0; i < island_number; i++)
    {
        sd += (sum[i] - average) * (sum[i] - average);
    }
    sd /= island_number;
    sd = sqrt(sd);

    // cout << average << ";" << sd << ";";
    result[0] = average;
    result[1] = sd;
    // DEBUG(printf("\nDensityPopulation\n"););
    // DEBUG(printf("%lf;%lf;\n", average, sd););
    free(sum);
    return result;
}

double densityWorld(populacao **populations, int island_number)
{
    DEBUG(printf("\ndensityWorld\n"););
    double total;
    double **sum = (double **)calloc(island_number, sizeof(double *));

    for (int i = 0; i < island_number; i++)
    {
        sum[i] = (double *)calloc(island_number, sizeof(double));
    }

    // print_population(populations[1]->individuos, populations[1]->size, 10, 1);
    // // print_population(populations[13]->individuos, populations[13]->size, 10, 1);
    // exit(0);

    // for all populations
    for (int i = 0; i < island_number; i++)
    {
        // in comparison with all other populations
        for (int j = i + 1; j < island_number; j++)
        {
            int nIndividualsI = populations[i]->size;
            int nIndividualsJ = populations[j]->size;

            // for all individuals from population i
            for (int k = 0; k < nIndividualsI; k++)
            {
                // against all individuals from the same population

                for (int l = 0; l < nIndividualsJ; l++)
                {
                    // sums with the norm-2 of individual j and k
                    sum[i][j] += euclidian(populations[i]->individuos[k], populations[j]->individuos[l], parameters.dimension);
                }
            }

            sum[i][j] = sqrt(sum[i][j]);

            // printf("sum[%d][%d] += %lf\n", i, j, sum[i][j]);
        }
    }

    for (int i = 0; i < island_number; i++)
    {
        for (int j = i; j < island_number; j++)
        {
            total += sum[i][j];
        }
    }
    // Errado?
    /*
    double sd = 0;

    total /= island_size;

    for (int i = 0; i < island_size; i++) {
        for(int j = i; j < island_size; j++)
         sd += (sum[i][j]-total)*(sum[i][j]-total);
    }
    sd /= island_size;
    sd = sqrt(sd);*/

    // DEBUG(printf("\nDensityWord\n"););
    // DEBUG(printf("%lf;\n", total););
    /*
    //cout << total << ";" << endl;

    // TODO: incompleto. Pensar numa maneira de comparar este número com o outro
    // pensamento: não é necessário comparar este número com o outro, só comparar
    // de uma execução com outra. Então, é somar tudo e retornar
    // verificar se esta ideia acima está correta
    */
    destroy_matriz(sum, island_number);
    return total;
}

void destroy_matriz(double **matriz, int size)
{
    if (matriz != NULL)
    {
        for (int i = 0; i < size; i++)
        {
            free(matriz[i]);
        }
        free(matriz);
    }
}

double convergence_calculation(populacao *population, double best)
{
    double count = 0;

    for (int i = 0; i < population->size; i++)
    {
        if (population->individuos[i].fitness == best)
        {
            count++;
        }
    }
    return count / population->size;
}

double convergence_calculation_islands(populacao **populations, int islands_size)
{
    double result;
    if (islands_size > 1)
    {
        result = densityWorld(populations, islands_size);
    }
    else
    {
        result = densityPopulation(populations, islands_size)[0];
    }
    return result;
}

int minimum(int a, int b)
{
    if (a > b)
        return b;
    return a;
}

int inInterval(double min, double max, double valor)
{
    if (valor >= min && valor <= max)
        return 1;
    return 0;
}