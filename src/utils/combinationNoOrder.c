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

// Função recursiva para gerar combinações e montá-las em um vetor
void generateCombinations(int n, int arr[], int index, int **result, int *size) {
    if (index == n) {
        result[*size] = (int *)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            result[*size][i] = arr[i];
        }
        (*size)++;
        return;
    }

    for (int i = (index == 0) ? 0 : arr[index - 1]; i < n; i++) {
        arr[index] = i;
        generateCombinations(n, arr, index + 1, result, size);
    }
}

int main() {
    int n;
    printf("Digite o tamanho do vetor (N): ");
    scanf("%d", &n);

    int arr[n];
    int *combinations[n*n*n*n]; // Alteramos o tamanho do vetor para evitar estouro de memória
    int size = 0;

    generateCombinations(n, arr, 0, combinations, &size);

    printf("Combinações possíveis:\n");
    printf("[");
    for (int i = 0; i < size; i++) {
        printVector(combinations[i], n);
        if (i < size - 1) {
            printf(", ");
        }
        free(combinations[i]);
    }
    printf("]\n");

    return 0;
}
