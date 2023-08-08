#include <stdio.h>
#include <stdlib.h>

// Função para imprimir um vetor em formato de lista
void printVector(int arr[], int n)
{

    for (int i = 0; i < n; i++)
    {
        printf("%d", arr[i]);
        if (i < n - 1)
        {
            printf(",");
        }
    }
}

// Função recursiva para gerar combinações e montá-las em um vetor
void generateCombinations(int n, int arr[], int index, int **result, int *size)
{
    if (index == n)
    {
        result[*size] = (int *)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++)
        {
            result[*size][i] = arr[i];
        }
        (*size)++;
        return;
    }

    for (int i = (index == 0) ? 0 : arr[index - 1]; i < n; i++)
    {
        arr[index] = i;
        generateCombinations(n, arr, index + 1, result, size);
    }
}

typedef struct array
{
    int **arr;
    int size;
} Array;

Array generateComb(int n)
{
    int arr[n];
    int **combinations = (int **)malloc(n * n * n * n * sizeof(int *));
    int size = 0;

    generateCombinations(n, arr, 0, combinations, &size);

    return (Array){combinations, size};
}

int main(int argc, char const *argv[])
{
    int n = atoi(argv[1]);

    Array result = generateComb(n);

    for (int i = 0; i < result.size; i++)
    {
        printVector(result.arr[i], n);
        if (i < result.size - 1)
            printf("\n");

        free(result.arr[i]);
    }

    return 0;
}
