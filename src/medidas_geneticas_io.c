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

typedef struct files_list_ {
    char **files;
    int num_files;
} files_list;

void print_string_vector(char **files, int num) {
    int i = 0;
    for (int i = 0; i < num; i++) {
        printf("file: %s\n", files[i]);
    }
}

char **alocc_string_matrix(int num_strings, int size_for_each_string) {
    char **matrix_string = (char **) malloc(num_strings * sizeof(char *));
    for (int i = 0; i < num_strings; i++) {
        matrix_string[i] = calloc(size_for_each_string, sizeof(char));
    }

    return matrix_string;
}

int i = 0;
void list_all_files_in_dir_(char *dirname, char **files) {

    DIR *d;
    struct dirent *dir;
    d = opendir(dirname);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (!strcmp(dir->d_name, "..") || !strcmp(dir->d_name, ".")) continue;
            char newPathName[100];
            sprintf(newPathName, "%s/%s", dirname, dir->d_name);
            if (dir->d_type == 4) //diretório
                list_all_files_in_dir_(newPathName, files);
            if (dir->d_type == 8 && strstr(newPathName, ".log")) { //file
                strcpy(files[i++], newPathName);
            }
        }
        closedir(d);
    }
}

files_list list_all_files_in_dir(char *dirname) {
    files_list files_list;
    files_list.files = alocc_string_matrix(10000, 50);
    list_all_files_in_dir_(dirname, files_list.files);
    files_list.num_files = i;
    return files_list;
}

// TODO: Essa função tem que processar cada arquivos em files_list encontrado pela função list_all_files_in_dir, 
// calcular as metricas para esse arquivo, e depois salvar isso em 
// outro arquivo de mesmo nome na pasta metrics.

void write_metrics_for_each_files(files_list files_list) {
    for (int i = 0; i < files_list.num_files; i++) {
        char *file = files_list.files[i];
        char *file_name = get_file_name(file);
        char *dir_name = get_dir_name(file);
        char *new_file_name = (char *) malloc(100 * sizeof(char));
        sprintf(new_file_name, "%s/%s", dir_name, file_name);
        printf("file: %s\n", new_file_name);
        write_metrics_for_each_file(new_file_name);
    }
}