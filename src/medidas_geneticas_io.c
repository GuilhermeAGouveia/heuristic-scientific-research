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

void print_string_vector(char **dirname) {

}

char* list_all_files_in_dir(char *dirname) {
    DIR *d;
    struct dirent *dir;
    d = opendir(dirname);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (!strcmp(dir->d_name, "..") || !strcmp(dir->d_name, ".")) continue;
            char newPathName[100];
            sprintf(newPathName, "%s/%s", dirname, dir->d_name);
            if (dir->d_type == 4) //diretório
                list_all_files_in_dir(newPathName);
            if (dir->d_type == 8) //file
                printf("%s\n", newPathName);
        }
        closedir(d);
    }
    return 0;
}

void main() {
    list_all_files_in_dir("./log");
}