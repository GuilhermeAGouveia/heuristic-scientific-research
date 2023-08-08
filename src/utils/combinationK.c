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

int main() {
    int n, k;

    printf("Digite o tamanho do vetor (N): ");
    scanf("%d", &n);
    
    printf("Digite o valor de K: ");
    scanf("%d", &k);

    int arr[n];
    int **combinations = NULL;
    int size = 0;

    generateCombinations(n, arr, 0, &combinations, &size, k);

    printf("Combinações possíveis:\n");
    printf("[");
    for (int i = 0; i < size; i++) {
        if (i > 0) {
            printf(", ");
        }
        printf("[");
        for (int j = 0; j < n; j++) {
            if (j > 0) {
                printf(", ");
            }
            printf("%d", combinations[i][j]);
        }
        printf("]");
        free(combinations[i]);
    }
    printf("]\n");
    
    free(combinations);

    return 0;
}
