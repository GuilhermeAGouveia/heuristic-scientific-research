#include <stdio.h>

// Função para trocar dois elementos do vetor
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Função recursiva para gerar permutações e calcular a soma
long long int generatePermutations(int arr[], int n, int index) {
    if (index == n - 1) {
        long long int sum = 0;
        for (int i = 0; i < n; i++) {
            sum += arr[i];
        }
        return sum;
    }

    long long int totalSum = 0;
    for (int i = index; i < n; i++) {
        // Troca o elemento atual com o elemento do índice 'index'
        swap(&arr[i], &arr[index]);
        // Chama recursivamente para o subarray restante e acumula a soma
        totalSum += generatePermutations(arr, n, index + 1);
        // Desfaz a troca para restaurar o array original
        swap(&arr[i], &arr[index]);
    }
    return totalSum;
}

int main() {
    int n;
    printf("Digite o tamanho do vetor (N): ");
    scanf("%d", &n);

    int arr[n];
    printf("Digite os elementos do vetor:\n");
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    long long int result = generatePermutations(arr, n, 0);
    printf("Soma das permutações: %lld\n", result);

    return 0;
}
