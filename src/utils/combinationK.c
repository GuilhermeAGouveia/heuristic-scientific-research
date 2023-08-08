#include <stdio.h>
#include <stdlib.h>

void generateCombinations(int n, int arr[], int index, int ***result, int *size, int k) {
    if (index == n) {
        *result = (int **)realloc(*result, (*size + 1) * sizeof(int *));
        (*result)[*size] = (int *)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            (*result)[*size][i] = arr[i];
        }
        (*size)++;
        return;
    }

    for (int i = (index == 0) ? 0 : arr[index - 1]; i <= k; i++) {
        arr[index] = i;
        generateCombinations(n, arr, index + 1, result, size, k);
    }
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage: %s <n> <k>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int k = atoi(argv[2]);

    int arr[n];
    int **combinations = NULL;
    int size = 0;

    generateCombinations(n, arr, 0, &combinations, &size, k);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < n; j++) {
            if (j > 0) {
                printf(",");
            }
            printf("%d", combinations[i][j]);
        }
        printf("\n");
        free(combinations[i]);
    }
    
    free(combinations);

    return 0;
}
