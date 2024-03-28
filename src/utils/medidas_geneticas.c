
#include "../libs/commom.h"
#include <dirent.h>

#define DEBUG(x)

void read_parameters_file(int epoca, int population);

typedef struct files_list_
{
    char **files;
    int num_files;
} files_list;

void destroy_files_list(files_list *files_destroy)
{
    for (int i = 0; i < files_destroy->num_files; i++)
    {
        free(files_destroy->files[i]);
    }
    free(files_destroy->files);
}

void destroy_matriz(double **matriz, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(matriz[i]);
    }
    free(matriz);
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
    files_list.files = alocc_string_matrix(100000, 257);
    list_all_files_in_dir_(dirname, files_list.files);
    files_list.num_files = i;
    return files_list;
}

/**
 * @brief Filter a files_list by epoca, generation and population
 * Se epoca < 0, não filtra por epoca
 * Se generation < 0, não filtra por generation
 * Se population < 0, não filtra por population
 * @param files_list_original lista original dos arquivos para filtrar
 * @param epoca valor da epoca para filtrar
 * @param generation valor da generation para filtrar
 * @param population valor da population para filtrar
 * @return files_list
 */
files_list filter_file_list_by(files_list files_list_original, int epoca, int generation, int population)
{
    DEBUG(printf("\nfilter_file_list_by\n"););
    files_list filtered_files;
    char current_file[257];
    filtered_files.num_files = 0;
    filtered_files.files = alocc_string_matrix(files_list_original.num_files, 257);

    for (int i = 0; i < files_list_original.num_files; i++)
    {
        char epoca_str[257];
        char generation_str[257];
        char population_str[257];

        // Filter by epoca
        sprintf(epoca_str, "epoca_%d", epoca);
        if (epoca < 0)
            strcpy(current_file, files_list_original.files[i]);
        else if (strstr(files_list_original.files[i], epoca_str))
        {
            DEBUG(printf("file: %s\n", files_list_original.files[i]););
            strcpy(current_file, files_list_original.files[i]);
        }
        else
            current_file[0] = '\0';

        // Filter by generation
        sprintf(generation_str, "generation_%d.log", generation);
        if (generation >= 0)
        {
            if (strstr(current_file, generation_str))
            {
                DEBUG(printf("file: %s\n", files_list_original.files[i]););
            }
            else
                current_file[0] = '\0';
        }

        sprintf(population_str, "population_%d", population);
        if (population >= 0)
        {
            if (strstr(current_file, population_str))
            {
                DEBUG(printf("file: %s\n", files_list_original.files[i]););
            }
            else
                current_file[0] = '\0';
        }

        // Add to filtered_files
        if (current_file[0] != '\0')
            strcpy(filtered_files.files[filtered_files.num_files++], current_file);
    }

    return filtered_files;
}

int extract_min_generations_from_epoca(files_list files_list_original, int epoca)
{
    files_list files_aux;
    files_aux = filter_file_list_by(files_list_original, epoca, 0, -1);
    int n_populations = files_aux.num_files;
    destroy_files_list(&files_aux);
    int min = (int)INFINITY;
    int current_value;

    for (int i = 0; i < n_populations; i++)
    {
        files_aux = filter_file_list_by(files_list_original, epoca, -1, i);
        current_value = files_aux.num_files;
        destroy_files_list(&files_aux);
        if (current_value < min)
            min = current_value;
    }

    return min;
}

populacao *read_population_from_generation_file(char *filename, int epoca, int population)
{
    read_parameters_file(epoca, population);
    // print_parameters(parameters);
    populacao *population_ = generate_clean_island(1, parameters.population_size, parameters.dimension);
    // print_population(population_->individuos, population_->size, parameters.dimension, 1);
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        DEBUG(printf("Error opening file!\n"););
        exit(1);
    }
    char line[1024];
    int i = 0;
    while (fgets(line, 1024, file))
    {
        // DEBUG(printf("read line: %d\n", i););
        char *tmp = strdup(line);
        char *chromossome_string = strtok(tmp, ">");
        // DEBUG(printf("primeira parte: %s\n", chromossome_string););
        char *fitness_string = strtok(NULL, ">");
        // DEBUG(printf("segunda parte: %s\n", fitness_string););
        population_->individuos[i].fitness = atof(fitness_string);
        char *chromossome_value_string = strtok(chromossome_string, " ");
        int j = 0;
        while (chromossome_value_string)
        {
            population_->individuos[i].chromosome[j++] = atof(chromossome_value_string);
            chromossome_value_string = strtok(NULL, " ");
        }
        i++;
        // DEBUG(printf("poa\n"););
        free(tmp);
    }

    fclose(file);
    DEBUG(printf("\n[end] read_population_from_generation_file\n"););
    return population_;
}

enum MetaInfo
{
    EPOCA,
    GENERATION,
    POPULATION
};

int get_metainfo_from_filename(char *filename, enum MetaInfo meta_info)
{
    char *tmp = strdup(filename);
    char *population_string = strtok(tmp, "/");
    char compareStrings[3][20] = {"epoca_", "generation_", "population_"};

    while (strstr(population_string, compareStrings[meta_info]) == NULL)
        population_string = strtok(NULL, "/");

    population_string = strtok(population_string, "_");
    population_string = strtok(NULL, "_");
    population_string = strtok(population_string, ".");
    DEBUG(printf("population_string: %s\n", population_string););
    free(tmp);
    return atoi(population_string);
}

populacao **mount_populations(files_list files)
{
    DEBUG(printf("\nmount_populations\n"););

    populacao **populations = calloc(files.num_files, sizeof(populacao *));
    for (int i = 0; i < files.num_files; i++)
    {
        DEBUG(printf("file: %s\n", files.files[i]););
        int population = get_metainfo_from_filename(files.files[i], POPULATION);
        int epoca = get_metainfo_from_filename(files.files[i], EPOCA);
        int generation = get_metainfo_from_filename(files.files[i], GENERATION);

        populations[i] = read_population_from_generation_file(files.files[i], epoca, population);
        // printf("file: %s\n", files.files[i]);
        // printf("epoca: %d\n", epoca);
        // printf("population: %d\n", population);
        // printf("generation: %d\n", generation);
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

// implements the same diversity metric of the density population
// extends it to the entire "world"
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

            printf("sum[%d][%d] += %lf\n", i, j, sum[i][j]);
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

void clean_metric_dir()
{
    DEBUG(printf("\nclean_log_dir\n"););

    system("rm -rf log/metricas/*");
}

void read_parameters_file(int epoca, int population)
{
    DEBUG(printf("\nread_parameters_file\n"););
    char parameters_filename[1024];
    sprintf(parameters_filename, "log/data/epoca_%d/population_%d/_parametros.dat", epoca, population);
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
    fclose(file);
    DEBUG(printf("\n[end] read_parameters_file\n"););
}

void write_metrics_for_each_files()
{
    read_parameters_file(0, 0);
    clean_metric_dir();
    DEBUG(printf("population_size: %d\n", parameters.population_size););
    DEBUG(printf("num_epocas: %d\n", parameters.num_epocas););
    DEBUG(printf("num_generations_per_epoca: %d\n", parameters.num_generations_per_epoca););

    FILE *output_metric;
    files_list all_files = list_all_files_in_dir("./log");
    char cmd[1024];
    for (int epoca = 0; epoca < parameters.num_epocas; epoca++)
    {
        sprintf(cmd, "mkdir -p log/metricas/epoca_%d/", epoca);
        system(cmd);
        char filename[1024];
        sprintf(filename, "log/metricas/epoca_%d/metrics_for_each_generation.dat", epoca);
        output_metric = fopen(filename, "w");
        int min_generations = extract_min_generations_from_epoca(all_files, epoca);

        for (int generation = 0; generation < min_generations; generation++)
        {
            files_list filtered_files = filter_file_list_by(all_files, epoca, generation, -1);
            DEBUG(print_string_vector(filtered_files.files, filtered_files.num_files););
            populacao **populations = mount_populations(filtered_files);
            double *densityPopulationResult = densityPopulation(populations, filtered_files.num_files);
            double densityWorldResult = densityWorld(populations, filtered_files.num_files);
            // Criar pasta
            DEBUG(printf("densityPopulationResult: %lf %lf\n", densityPopulationResult[0], densityPopulationResult[1]););
            DEBUG(printf("densityWorldResult: %lf\n", densityWorldResult););

            if (output_metric == NULL)
            {
                DEBUG(printf("Error opening file!\n"););
                exit(1);
            }
            fprintf(output_metric, "%lf ", densityPopulationResult[0]);
            fprintf(output_metric, "%lf ", densityPopulationResult[1]);
            fprintf(output_metric, "%lf\n", densityWorldResult);
            destroy_island(*populations, filtered_files.num_files - 1);
            destroy_files_list(&filtered_files);
            free(densityPopulationResult);
        }
        fclose(output_metric);
    }
    destroy_files_list(&all_files);
}

int main()
{
    write_metrics_for_each_files();
}
