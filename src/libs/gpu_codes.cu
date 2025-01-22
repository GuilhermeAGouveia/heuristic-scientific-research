#include <cuda_runtime.h>
#include "../libs/types.h"
#include "../libs/gpu_codes.h"
#include <stdio.h>


#define DIMENSION 10

// Função para calcular a distância euclidiana (no dispositivo)
__device__ double euclidian(double *a, double *b, int dim) {
    double sum = 0;
    for (int i = 0; i < dim; i++) {
        double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

// Kernel para calcular a densidade da população
__global__ void densityPopulationKernel(populacao **populations, double *sum, int island_number) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx >= island_number) return; // Garantir índice válido

    int nIndividuals = populations[idx]->size;
    for (int j = 0; j < nIndividuals - 1; j++) {
        for (int k = j + 1; k < nIndividuals; k++) {
            sum[idx] += euclidian(
                populations[idx]->individuos[j].chromosome,
                populations[idx]->individuos[k].chromosome,
                DIMENSION
            );
        }
    }
}

typedef struct GpuMemoryNode {
    void *address;
    struct GpuMemoryNode *next;
} GpuMemoryNode;

GpuMemoryNode *gpuMemoryHead = NULL;

// Função para registrar um endereço de memória na lista
void registerGpuMemory(void *address) {
    GpuMemoryNode *newNode = (GpuMemoryNode *)malloc(sizeof(GpuMemoryNode));
    newNode->address = address;
    newNode->next = gpuMemoryHead;
    gpuMemoryHead = newNode;
}

// Função para desalocar toda a memória registrada na GPU
void freeAllGpuMemory() {
    GpuMemoryNode *current = gpuMemoryHead;
    while (current) {
        cudaFree(current->address);
        GpuMemoryNode *temp = current;
        current = current->next;
        free(temp);
    }
    gpuMemoryHead = NULL;
}

populacao **temp2;
cudaError_t allocateMemoryOnGPU(populacao **populations, int island_number, populacao ***d_populations_out) {
    populacao **d_populations;
    populacao **tempPopulacoes = (populacao **)malloc(island_number * sizeof(populacao *));
    if (tempPopulacoes == NULL) {
        fprintf(stderr, "Erro ao alocar memória temporária na CPU.\n");
        return cudaErrorMemoryAllocation;
    }

    // Alocar memória para o vetor de populações na GPU
    cudaError_t err = cudaMalloc((void**)&d_populations, island_number * sizeof(populacao *));
    registerGpuMemory(d_populations);
    if (err != cudaSuccess) {
        fprintf(stderr, "Erro ao alocar memória para populações na GPU: %s\n", cudaGetErrorString(err));
        free(tempPopulacoes);
        return err;
    }

    for (int i = 0; i < island_number; i++) {
        individuo *d_individuos;
        populacao *d_populacao;

        // Alocar memória para a população e seus indivíduos na GPU
        err = cudaMalloc(&d_populacao, sizeof(populacao));
        registerGpuMemory(d_populacao);

        err = cudaMalloc(&d_individuos, populations[i]->size * sizeof(individuo));
        registerGpuMemory(d_individuos);
        // Copiar os cromossomos de cada indivíduo para a GPU
        for (int j = 0; j < populations[i]->size; j++) {
            double *d_chromosome;

            err = cudaMalloc(&d_chromosome, DIMENSION * sizeof(double));
            registerGpuMemory(d_chromosome);

            err = cudaMemcpy(d_chromosome, populations[i]->individuos[j].chromosome,
                             DIMENSION * sizeof(double), cudaMemcpyHostToDevice);
            free(populations[i]->individuos[j].chromosome);
            free(populations[i]->individuos[j].velocidade);
            // Atualizar o ponteiro do cromossomo no indivíduo
            populations[i]->individuos[j].chromosome = d_chromosome;
        }

        // Copiar os indivíduos para a GPU
        err = cudaMemcpy(d_individuos, populations[i]->individuos,
                         populations[i]->size * sizeof(individuo), cudaMemcpyHostToDevice);
        free(populations[i]->individuos);
        free(populations[i]->neighbours);
        // Atualizar o ponteiro de indivíduos na população auxiliar
        tempPopulacoes[i] = populations[i];
        tempPopulacoes[i]->individuos = d_individuos;

        // Copiar a população auxiliar para a GPU
        err = cudaMemcpy(d_populacao, tempPopulacoes[i], sizeof(populacao), cudaMemcpyHostToDevice);
        free(tempPopulacoes[i]);
        // Atualizar o vetor de populações na GPU
        tempPopulacoes[i] = d_populacao;
    }

    // Copiar o vetor de populações para a GPU
    err = cudaMemcpy(d_populations, tempPopulacoes, island_number * sizeof(populacao *), cudaMemcpyHostToDevice);


    free(tempPopulacoes);
    *d_populations_out = d_populations;
    return cudaSuccess;
}


// Função principal
double densityPopulation(populacao **populations, int island_number) {
    populacao **d_populations;

    // Alocar memória e inicializar dados na GPU
    cudaError_t err = allocateMemoryOnGPU(populations, island_number, &d_populations);
    if (err != cudaSuccess) {
        return -1;
    }


    // Alocar e zerar memória para os resultados na GPU
    double *d_sum;
    cudaMalloc(&d_sum, island_number * sizeof(double));
    cudaMemset(d_sum, 0, island_number * sizeof(double));


    // Configurar e lançar o kernel
    int blockSize = 256;
    int gridSize = (island_number + blockSize - 1) / blockSize;
    densityPopulationKernel<<<gridSize, blockSize>>>(d_populations, d_sum, island_number);
    cudaDeviceSynchronize();

    // Recuperar resultados do dispositivo
    double *sum = (double *)malloc(island_number * sizeof(double));
    cudaMemcpy(sum, d_sum, island_number * sizeof(double), cudaMemcpyDeviceToHost);

    // Calcular a média
    double average = 0;
    for (int i = 0; i < island_number; i++) {
        average += sum[i];
    }
    average /= island_number;

    // Desalocar memória
    free(sum);
    cudaFree(d_sum);
    //cudaFree(d_populations);
    temp2 = d_populations;
    return average;
}

__global__ void densityWorldKernel(populacao **populations, double *sum_flat, int island_number, int dimension) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    int i = idx / island_number;
    int j = idx % island_number;

    if (i < island_number && j > i) {
        int nIndividualsI = populations[i]->size;
        int nIndividualsJ = populations[j]->size;

        double local_sum = 0.0;

        for (int k = 0; k < nIndividualsI; k++) {
            for (int l = 0; l < nIndividualsJ; l++) {
                local_sum += euclidian(
                    populations[i]->individuos[k].chromosome,
                    populations[j]->individuos[l].chromosome,
                    dimension
                );
            }
        }

        // Atualiza a soma total de forma segura
        atomicAdd(&sum_flat[i * island_number + j], sqrt(local_sum));
    }
}


double densityWorld(populacao **populations, int island_number) {
    double *d_sum_flat, total = 0;
    populacao **d_populations = temp2;

    // Alocar memória na GPU para populações
    //cudaMalloc(&d_populations, island_number * sizeof(populacao *));
    //cudaMemcpy(d_populations, populations, island_number * sizeof(populacao *), cudaMemcpyHostToDevice);

    // Alocar memória contínua para sum_flat na GPU
    cudaMalloc(&d_sum_flat, island_number * island_number * sizeof(double));
    cudaMemset(d_sum_flat, 0, island_number * island_number * sizeof(double));

    // Configurar e lançar o kernel
    int blockSize = 256;
    int gridSize = (island_number * island_number + blockSize - 1) / blockSize;
    densityWorldKernel<<<gridSize, blockSize>>>(d_populations, d_sum_flat, island_number, DIMENSION);
    cudaDeviceSynchronize();

    // Recuperar resultados do dispositivo
    double *h_sum_flat = (double *)malloc(island_number * island_number * sizeof(double));
    cudaMemcpy(h_sum_flat, d_sum_flat, island_number * island_number * sizeof(double), cudaMemcpyDeviceToHost);

    // Calcular o total
    for (int i = 0; i < island_number; i++) {
        for (int j = i + 1; j < island_number; j++) {
            total += h_sum_flat[i * island_number + j];
        }
    }

    // Limpar memória
    free(h_sum_flat);
    cudaFree(d_sum_flat);
    cudaFree(d_populations);
    free(populations);
    freeAllGpuMemory();
    return total;
}
