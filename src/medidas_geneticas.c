#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#define NO_RECORDING
#include "./libs/funcoes_cec_2015/cec15_test_func.h"
#include "./libs/statistics.h"
#include "./libs/types.h"
#include "./libs/utils.h"
#include "./libs/crossover.h"
#include "./libs/log.h"
#include <dirent.h>
#include <string.h>

#define DEBUG(x)

static args parameters;

typedef struct files_list_
{
    char **files;
    int num_files;
} files_list;

individuo *generate_population(int n_individuos, int dimension, domain domain_function)
{
    DEBUG(printf("\ngenerate_population\n"););
    individuo *population = malloc(n_individuos * sizeof(individuo));
    for (int i = 0; i < n_individuos; i++)
    {
        population[i].chromosome = (double *)malloc(dimension * sizeof(double));
        for (int j = 0; j < dimension; j++)
        {
            population[i].chromosome[j] = random_double(domain_function.min, domain_function.max);
        }
    }
    return population;
}

populacao *generate_island(int island_size, int population_size, int dimension, domain domain_function)
{
    DEBUG(printf("\ngenerate_island\n"););
    populacao *populations = malloc(island_size * sizeof(populacao));
    populacao **neighbours = calloc(4, sizeof(populacao *));
    for (int i = 0; i < island_size; i++)
    {
        populations[i].individuos = generate_population(population_size, dimension, domain_function);
        populations[i].size = population_size;
        populations[i].crossover = rand() % 6;
        populations[i].neighbours = calloc(4, sizeof(populacao *));
        populations[i].neighbours[0] = &populations[(i + 1) % island_size]; // talvez isso dê problema
        populations[i].neighbours[1] = &populations[(i + 3) % island_size]; // talvez isso dê problema
    }
    return populations;
}

void print_string_vector(char **files, int num)
{
    int i = 0;
    for (int i = 0; i < num; i++)
    {
        printf("file: %s\n", files[i]);
    }
    printf("num_files: %d\n", num);
}

char **alocc_string_matrix(int num_strings, int size_for_each_string)
{
    DEBUG(puts("\nalocc_string_matrix\n"););
    char **matrix_string = (char **)malloc(num_strings * sizeof(char *));
    for (int i = 0; i < num_strings; i++)
    {
        matrix_string[i] = calloc(size_for_each_string, sizeof(char));
    }

    DEBUG(puts("\n[end] alocc_string_matrix\n"););
    return matrix_string;
}

int i = 0;
void list_all_files_in_dir_(char *dirname, char **files)
{

    DIR *d;
    struct dirent *dir;
    d = opendir(dirname);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (!strcmp(dir->d_name, "..") || !strcmp(dir->d_name, "."))
                continue;
            char newPathName[257];
            sprintf(newPathName, "%s/%s", dirname, dir->d_name);
            if (dir->d_type == 4) // diretório
                list_all_files_in_dir_(newPathName, files);
            if (dir->d_type == 8 && strstr(newPathName, ".log"))
            { // file
                strcpy(files[i++], newPathName);
            }
        }
        closedir(d);
    }
}

files_list list_all_files_in_dir(char *dirname)
{
    DEBUG(printf("\nlist_all_files_in_dir\n"););
    files_list files_list;
    files_list.files = alocc_string_matrix(10000, 257);
    list_all_files_in_dir_(dirname, files_list.files);
    files_list.num_files = i;
    return files_list;
}

files_list filter_file_list_by(files_list files_list_original, int epoca, int generation)
{
    DEBUG(printf("\nfilter_file_list_by\n"););
    files_list filtered_files;
    filtered_files.num_files = 0;
    filtered_files.files = alocc_string_matrix(10, 257);

    for (int i = 0; i < files_list_original.num_files; i++)
    {
        char epoca_str[257];
        char generation_str[257];
        sprintf(epoca_str, "epoca_%d", epoca);
        sprintf(generation_str, "generation_%d.log", generation);
        if (strstr(files_list_original.files[i], epoca_str) && strstr(files_list_original.files[i], generation_str))
        {
            DEBUG(printf("file: %s\n", files_list_original.files[i]););
            strcpy(filtered_files.files[filtered_files.num_files++], files_list_original.files[i]);
        }
    }
    return filtered_files;
}

individuo *read_population_from_generation_file(char *filename, int population_size, int dimension)
{
    individuo *generation = generate_population(population_size, dimension, (domain){0, 0});

    FILE *file = fopen(filename, "r");
    char line[1024];
    int i = 0;
    while (fgets(line, 1024, file))
    {

        char *tmp = strdup(line);
        char *chromossome_string = strtok(tmp, ">");
        // printf("primeira parte: %s\n", chromossome_string);
        char *fitness_string = strtok(NULL, ">");
        // printf("segunda parte: %s\n", fitness_string);
        generation[i].fitness = atof(fitness_string);
        char *chromossome_value_string = strtok(chromossome_string, " ");
        int j = 0;
        while (chromossome_value_string)
        {
            generation[i].chromosome[j++] = atof(chromossome_value_string);
            chromossome_value_string = strtok(NULL, " ");
        }
        i++;
        free(tmp);
    }
    fclose(file);
    return generation;
}

populacao *mount_populations(files_list files)
{
    DEBUG(printf("\nmount_populations\n"););
    int population_size = parameters.population_size;
    populacao *populations = generate_island(files.num_files, population_size, 10, (domain){0, 0});
    for (int i = 0; i < files.num_files; i++)
    {
        DEBUG(printf("file: %s\n", files.files[i]););
        populations[i].individuos = read_population_from_generation_file(files.files[i], population_size, 10);
    }
    return populations;
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

double *densityPopulation(populacao *populations, int island_size)
{
    double average = 0;
    double sd = 0;
    double *sum = (double *)calloc(island_size, sizeof(double));
    double sumIndividual;
    int nIndividuals = populations[0].size;
    double multiplier = 2 / (nIndividuals - 1);
    double *result = malloc(2 * sizeof(double));
    // for all populations
    for (int i = 0; i < island_size; i++)
    {
        // for all individuals from population i
        for (int j = 0; j < nIndividuals - 1; j++)
        {
            // against all individuals from the same population
            for (int k = j + 1; k < nIndividuals; k++)
            {
                // sums with the norm-2 of individual j and k
                sum[i] += euclidian(populations[i].individuos[j], populations[i].individuos[k], 10);
            }
        }
    }

    for (int i = 0; i < island_size; i++)
    {
        average += sum[i];
        // printf("%lf");
    }

    average /= island_size;

    for (int i = 0; i < island_size; i++)
    {
        sd += (sum[i] - average) * (sum[i] - average);
    }
    sd /= island_size;
    sd = sqrt(sd);

    // cout << average << ";" << sd << ";";
    result[0] = average;
    result[1] = sd;
    // printf("\nDensityPopulation\n");
    // printf("%lf;%lf;\n", average, sd);
    return result;
}

// implements the same diversity metric of the density population
// extends it to the entire "world"
double densityWorld(populacao *populations, int island_size)
{
    double total;
    double sum[island_size][island_size];
    int nIndividuals = populations[0].size;

    // for all populations
    for (int i = 0; i < island_size; i++)
    {
        // in comparison with all other populations
        for (int j = i; j < island_size; j++)
        {
            // for all individuals from population i
            for (int k = 0; k < nIndividuals - 1; k++)
            {
                // against all individuals from the same population
                for (int l = j + 1; l < nIndividuals; l++)
                {
                    // sums with the norm-2 of individual j and k
                    sum[i][j] += euclidian(populations[i].individuos[k], populations[j].individuos[l], 10);
                }
            }
        }
    }

    for (int i = 0; i < island_size; i++)
    {
        for (int j = i; j < island_size; j++)
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

    // printf("\nDensityWord\n");
    // printf("%lf;\n", total);
    /*
    //cout << total << ";" << endl;

    // TODO: incompleto. Pensar numa maneira de comparar este número com o outro
    // pensamento: não é necessário comparar este número com o outro, só comparar
    // de uma execução com outra. Então, é somar tudo e retornar
    // verificar se esta ideia acima está correta
    */
    return total;
}
/*
Parameters:
 function_number: 1,
 F: 0.990000,
 time_limit: 10,
 island_size: 5,
 population_size: 10,
 dimension: 10,
 domain function interval: [-100.000000, 100.000000],
 num_generations: 300,
 mutation_rate: 100,
 crossover_rate: 100,
 num_migrations: 3
 */

void clean_metric_dir()
{
    DEBUG(printf("\nclean_log_dir\n"););

    system("rm -rf log/metricas/*");
}

void read_parameters_file(char *parameters_filename)
{
    FILE *file = fopen(parameters_filename, "r");
    char line[1024];
    int i = 0;
    fgets(line, 1024, file);
    while (fgets(line, 1024, file))
    {
        if (strstr(line, "population_size"))
        {
            sscanf(line, " population_size: %d", &parameters.population_size);
        }

        if (strstr(line, "dimension"))
        {
            sscanf(line, " dimension: %d", &parameters.dimension);
        }

        if (strstr(line, "island_size"))
        {
            sscanf(line, " island_size: %d", &parameters.island_size);
        }
        if (strstr(line, "num_epocas"))
        {
            sscanf(line, " num_epocas: %d", &parameters.num_epocas);
        }
        if (strstr(line, "num_generations"))
        {
            sscanf(line, " num_generations: %d", &parameters.num_generations_per_epoca);
        }
    }
}

void write_metrics_for_each_files()
{
    read_parameters_file("./log/data/_parametros.dat");
    clean_metric_dir();
    printf("population_size: %d\n", parameters.population_size);
    printf("num_epocas: %d\n", parameters.num_epocas);
    printf("num_generations_per_epoca: %d\n", parameters.num_generations_per_epoca);

    FILE *output_metric;
    files_list all_files = list_all_files_in_dir("./log");
    char cmd[1024];
    for (int i = 0; i < parameters.num_epocas; i++)
    {
        sprintf(cmd, "mkdir -p log/metricas/epoca_%d/", i);
        system(cmd);
        char filename[1024];
        sprintf(filename, "log/metricas/epoca_%d/metrics_for_each_generation.dat", i);
        output_metric = fopen(filename, "w");

        for (int j = 0; j < parameters.num_generations_per_epoca; j++)
        {
            files_list filtered_files = filter_file_list_by(all_files, i, j);
            DEBUG(print_string_vector(filtered_files.files, filtered_files.num_files););
            populacao *populations = mount_populations(filtered_files);
            double *densityPopulationResult = densityPopulation(populations, filtered_files.num_files);
            double densityWorldResult = densityWorld(populations, filtered_files.num_files);
            // Criar pasta

            if (output_metric == NULL)
            {
                printf("Error opening file!\n");
                exit(1);
            }
            fprintf(output_metric, "%lf ", densityPopulationResult[0]);
            fprintf(output_metric, "%lf ", densityPopulationResult[1]);
            fprintf(output_metric, "%lf\n", densityWorldResult);
        }
        fclose(output_metric);
    }
}

int main()
{
    write_metrics_for_each_files();
}
