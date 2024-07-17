#include <sys/stat.h>
#include "../libs/commom.h"
#include <dirent.h>

//#define MAX_GEN 40000
#define MAX_PATH_LENGTH 1000
#define MAX_LINE_LENGTH 1000

void read_parameters_file(int function, int population, char *folderPath)
{
    DEBUG(printf("\nread_parameters_file\n"););
    char parameters_filename[1024];
    sprintf(parameters_filename, "%s/_parametros_F%d.dat", folderPath, function);
    //printf("\n%s/_parametros.dat", folderPath);
    FILE *file = fopen(parameters_filename, "r");
    if( file == NULL){
       printf("\nErro ao ler _parametros.dat");
       exit(0);
    }
    char line[1024];
    int i = 0;
    fgets(line, 1024, file);
    while (fgets(line, 1024, file))
    {

        if (strstr(line, "num_generations"))
        {
            sscanf(line, " num_generations: %d", &parameters.num_generations_per_epoca);
        }
    }
    fclose(file);
    DEBUG(printf("\n[end] read_parameters_file\n"););
    remove(parameters_filename);
}

void calculate_metrics(char *folderPath, int function, int num_executions)
{
    char path[MAX_PATH_LENGTH];
    int gen = parameters.num_generations_per_epoca;
    double *arrayD_Pop = (double *)calloc(parameters.num_generations_per_epoca, sizeof(double));
    double *arrayD_World = (double *)calloc(parameters.num_generations_per_epoca, sizeof(double));
    double *arrayD_Pop_sd = (double *)calloc(parameters.num_generations_per_epoca, sizeof(double));
    double *arrayD_World_sd = (double *)calloc(parameters.num_generations_per_epoca, sizeof(double));

    // Obtem a soma dos valores de cada geração
    for (int i = 1; i <= num_executions; i++)
    {
        FILE *file;
        sprintf(path, "%s/execucao_%d/metrics_F%d.txt", folderPath, i, function);
        file = fopen(path, "r");
        if (file != NULL)
        {
            int current_gen = 0;
            char line[MAX_LINE_LENGTH];
            while (fgets(line, sizeof(line), file) != NULL && current_gen < parameters.num_generations_per_epoca)
            {
                double density_pop, density_world;
                sscanf(line, "%lf %lf", &density_pop, &density_world);

                arrayD_Pop[current_gen] += density_pop;
                arrayD_World[current_gen] += density_world;

                current_gen++;
            }
            fclose(file);

            if (current_gen < gen)
            {
                gen = current_gen;
            }
        }
    }

    // Obtem a média
    for (int i = 0; i < gen; i++)
    {
        arrayD_Pop[i] /= num_executions;
        arrayD_World[i] /= num_executions;
    }

    // Obtem o desvio padrão
    for (int i = 1; i <= num_executions; i++)
    {
        FILE *file;
        sprintf(path, "%s/execucao_%d/metrics_F%d.txt", folderPath, i, function);
        file = fopen(path, "r");
        if (file != NULL)
        {
            int current_gen = 0;
            char line[MAX_LINE_LENGTH];
            while (fgets(line, sizeof(line), file) != NULL && current_gen < parameters.num_generations_per_epoca)
            {
                double density_pop, density_world;
                sscanf(line, "%lf %lf", &density_pop, &density_world);

                arrayD_Pop_sd[current_gen] += pow((density_pop - arrayD_Pop[current_gen]), 2);
                arrayD_World_sd[current_gen] += pow((density_world - arrayD_World[current_gen]), 2);

                current_gen++;
            }
            fclose(file);
        }
    }


    sprintf(path, "%s/metrics_all_executions", folderPath);
    mkdir(path, 0777);


    sprintf(path, "%s/metrics_all_executions/average_sd_F%d.csv", folderPath, function);
    FILE *output = fopen(path, "w");
    if (output != NULL)
    {
        fprintf(output, "P_Average,P_SD,W_Average,W_SD\n");
        for (int i = 0; i < gen; i++)
        {
            arrayD_Pop_sd[i] = sqrt(arrayD_Pop_sd[i] / num_executions);
            arrayD_World_sd[i] = sqrt(arrayD_World_sd[i] / num_executions);
            fprintf(output, "%.6f,%.6f,%.6f,%.6f\n", arrayD_Pop[i], arrayD_Pop_sd[i], arrayD_World[i], arrayD_World_sd[i]);
        }
        fclose(output);
    }
    free(arrayD_Pop);
    free(arrayD_Pop_sd);
    free(arrayD_World);
    free(arrayD_World_sd);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s folderPath function num_executions\n", argv[0]);
        return 1;
    }

    char *folderPath = argv[1];
    int function = atoi(argv[2]);
    int num_executions = atoi(argv[3]);

    read_parameters_file(function, 0, folderPath);

    calculate_metrics(folderPath, function, num_executions);

    return 0;
}
