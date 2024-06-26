#include <sys/stat.h>
#include "../libs/commom.h"
#include <dirent.h>

char *base_path = "logs_genetica/perguntas_artigo";

void print_matriz(double **matriz, int lin, int col)
{
    for (int i = 0; i < lin; i++)
    {
        for (int j = 0; j < col; j++)
        {
           printf("%lf ", matriz[i][j]);
        }
        puts("");
    }
}

double **alloc_matriz(int lines, int columns)
{
    double **matriz = (double **)calloc(lines, sizeof(double *));
    for (int i = 0; i < lines; i++)
    {
        matriz[i] = (double *)calloc(columns, sizeof(double));
    }
    return matriz;
}

double **dealloc_matriz(double **matriz, int lines)
{

    for (int i = 0; i < lines; i++)
    {
        free(matriz[i]);
    }
    free(matriz);
}

int min_lines_files(char **folders_path, int num_folders, int func)
{
    int min = 99999999;
    char file_name[200], linha[50];
    for (int i = 1; i <= num_folders; i++)
    {
        int total_lines = 0;
        sprintf(file_name, "%s/%s/average_sd_F%d.csv", base_path, folders_path[i], func);
        // printf("\nFolder: %s", file_name);
        FILE *file = fopen(file_name, "r");
        if (file == NULL)
        {
            fprintf(stderr, "Erro ao abrir o arquivo(min_lines_files)\n");
            exit(1);
        }

        while (fgets(linha, sizeof(linha), file) != NULL)
        {
            total_lines++;
        }
        if (total_lines < min)
            min = total_lines;
        fclose(file);
        // printf("total_lines: %d", total_lines);
    }
    return min - 1;
}

void filter_columns(int num_folders, char **folders_path, int num_func)
{
    char file_name[200], output_file_name[100];
    char *output_path = "logs_genetica/output_plot";
    //char *first_line_output = "P_Average,P_SD,W_Average,W_SD";

    for (int i = 1; i <= num_func; i++)
    {
        int min_lines = min_lines_files(folders_path, num_folders, i);

        //printf("\nMin_lines:%d", min_lines);

        double **D_Pop = alloc_matriz(min_lines, num_folders);
        double **D_Pop_sd = alloc_matriz(min_lines, num_folders);
        double **D_World = alloc_matriz(min_lines, num_folders);
        double **D_World_sd = alloc_matriz(min_lines, num_folders);

        // Read data
        for (int j = 0; j < num_folders; j++)
        {
            sprintf(file_name, "%s/%s/average_sd_F%d.csv", base_path, folders_path[j + 1], i);
            FILE *file = fopen(file_name, "r");

            if (file == NULL)
            {
                fprintf(stderr, "Erro ao abrir o arquivo (Filtered_columns)\n");
                exit(1);
            }
            int column = j;
            char line[100];
            fgets(line, sizeof(line), file);
            for (int k = 0; k < min_lines; k++)
            {
                fgets(line, sizeof(line), file);
                sscanf(line, "%lf,%lf,%lf,%lf", &D_Pop[k][column], &D_Pop_sd[k][column], &D_World[k][column], &D_World_sd[k][column]);
            }
            fclose(file);
        }
        
        //print_matriz(D_Pop, min_lines, num_folders);
        // Write data
        sprintf(file_name, "%s/P_Average/P_Average_plot_F%d.csv", output_path, i);
        FILE *output_D_Pop = fopen(file_name, "w");
        //fprintf(output_D_Pop, "%s\n", first_line_output);

        sprintf(file_name, "%s/P_SD/P_SD_plot_F%d.csv", output_path, i);
        FILE *output_D_Pop_sd = fopen(file_name, "w");
        //fprintf(output_D_Pop_sd, "%s\n", first_line_output);

        sprintf(file_name, "%s/World_Average/World_Average_plot_F%d.csv", output_path, i);
        FILE *output_D_World = fopen(file_name, "w");
        //fprintf(output_D_World, "%s\n", first_line_output);

        sprintf(file_name, "%s/World_SD/World_SD_plot_F%d.csv", output_path, i);
        FILE *output_D_World_sd = fopen(file_name, "w");
        //fprintf(output_D_World_sd, "%s\n", first_line_output);

        for (int j = 0; j < min_lines; j++)
        {
            char D_Pop_line[10000], D_Pop_sd_line[10000], D_World_line[10000], D_World_sd_line[10000], number[40];

            for (int k = 0; k < num_folders; k++)
            {
                sprintf(number, "%lf", D_Pop[j][k]);
                strcat(D_Pop_line, number);

                sprintf(number, "%lf", D_Pop_sd[j][k]);
                strcat(D_Pop_sd_line, number);

                sprintf(number, "%lf", D_World[j][k]);
                strcat(D_World_line, number);

                sprintf(number, "%lf", D_World_sd[j][k]);
                strcat(D_World_sd_line, number);

                if (k != num_folders - 1)
                {
                    strcat(D_Pop_line, ",");
                    strcat(D_Pop_sd_line, ",");
                    strcat(D_World_line, ",");
                    strcat(D_World_sd_line, ",");
                }
            }
            fprintf(output_D_Pop, "%s\n", D_Pop_line);
            fprintf(output_D_Pop_sd, "%s\n", D_Pop_sd_line);
            fprintf(output_D_World, "%s\n", D_World_line);
            fprintf(output_D_World_sd, "%s\n", D_World_sd_line);

            D_Pop_line[0] = '\0';
            D_Pop_sd_line[0] = '\0';
            D_World_line[0] = '\0';
            D_World_sd_line[0] = '\0';
        }

        dealloc_matriz(D_Pop, min_lines);
        dealloc_matriz(D_Pop_sd, min_lines);
        dealloc_matriz(D_World, min_lines);
        dealloc_matriz(D_World_sd, min_lines);
        fclose(output_D_Pop);
        fclose(output_D_Pop_sd);
        fclose(output_D_World);
        fclose(output_D_World_sd);
    }
}

int main(int argc, char *argv[])
{
    int num_func = 14, num_folders = argc - 1;
    filter_columns(num_folders, argv, num_func);
}