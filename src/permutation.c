#include <stdio.h>
#include <stdlib.h>

// Função para imprimir um vetor em formato de lista
void printVector(int arr[], int n) {
    printf("[");
    for (int i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) {
            printf(", ");
        }
    }
    printf("]");
}

// Função recursiva para gerar permutações e montá-las em um vetor
void generatePermutations(int n, int arr[], int index, int ***result, int *size) {
    if (index == n) {
        (*result) = (int **)realloc(*result, (*size + 1) * sizeof(int *));
        (*result)[*size] = (int *)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            (*result)[*size][i] = arr[i];
        }
        (*size)++;
        return;
    }

    for (int i = 0; i < n; i++) {
        arr[index] = i;
        generatePermutations(n, arr, index + 1, result, size);
    }
}

int main() {
    int n;
    printf("Digite o tamanho do vetor (N): ");
    scanf("%d", &n);

    int arr[n];
    int **permutations = NULL;
    int size = 0;

    generatePermutations(n, arr, 0, &permutations, &size);

    printf("Permutações possíveis:\n");
    printf("[");
    for (int i = 0; i < size; i++) {
        printVector(permutations[i], n);
        if (i < size - 1) {
            printf(", ");
        }
        free(permutations[i]);
    }
    printf("]\n");

    free(permutations);

    return 0;
}
