#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 1000        // Número máximo de nós no grafo
#define M 50         // Número máximo de formigas
#define ALPHA 1      // Influência da trilha de feromônio na escolha do próximo nó
#define BETA 2       // Influência da heurística na escolha do próximo nó
#define RHO 0.5      // Taxa de evap
#define Q 100        // Quantidade de feromônio depositado por formiga
#define MAX_ITER 100 // Número máximo de iterações

// Definição das estruturas de dados
typedef struct
{
    double x;
    double y;
} Node;

typedef struct
{
    int from;
    int to;
    double pheromone;
} Edge;

typedef struct
{
    int path[N];
    double length;
} Solution;

typedef struct
{
    int current_node;
    int visited[N];
} Ant;

// Função para inicializar as trilhas de feromônio
void init_pheromone_trails(double trails[][N])
{
    int i, j;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            trails[i][j] = 0.1; // Valor padrão inicial
        }
    }
}

// Função para calcular a heurística entre dois nós
double calculate_heuristic(Node node1, Node node2)
{
    // Substituir pela função matemática em questão
    return sqrt(pow(node1.x - node2.x, 2) + pow(node1.y - node2.y, 2));
}

// Função para escolher o próximo nó baseado nas trilhas de feromônio e na heurística
int choose_next_node(Ant ant, double trails[][N], Node nodes[])
{
    int i;
    double probabilities[N];
    double total_prob = 0.0;
    for (i = 0; i < N; i++)
    {
        if (ant.visited[i] == 0)
        {
            double pheromone = trails[ant.current_node][i];
            double heuristic = calculate_heuristic(nodes[ant.current_node], nodes[i]);
            probabilities[i] = pow(pheromone, ALPHA) * pow(heuristic, BETA);
            total_prob += probabilities[i];
        }
    }
    double r = (double)rand() / RAND_MAX;
    double sum = 0.0;
    for (i = 0; i < N; i++)
    {
        if (ant.visited[i] == 0)
        {
            sum += probabilities[i] / total_prob;
            if (r <= sum)
            {
                return i;
            }
        }
    }
    return -1; // Erro
}

// Função para construir uma solução para o problema
Solution construct_solution(double trails[][N], Node nodes[])
{
    int i, j;
    Solution solution;
    Ant ants[M];
    for (i = 0; i < M; i++)
    {
        ants[i].current_node = rand() % N; // Escolhe um nó aleatório para começar
        for (j = 0; j < N; j++)
        {
            ants[i].visited[j] = 0;
        }
        ants[i].visited[ants[i].current_node] = 1;
    }
    for (i = 1; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            int next_node = choose_next_node(ants[j], trails, nodes);
            ants[j].visited[next_node] = 1;
            ants[j].current_node = next_node;
            solution.path[i - 1] = next_node;
        }
    }
    solution.path[N - 1] = solution.path[0]; // Volta para o nó de origem
    solution.length = 0.0;
    for (i = 0; i < N - 1; i++)
    {
        solution.length += calculate_heuristic(nodes[solution.path[i - 1]], nodes[solution.path[i]]);
    }
    return solution;
}

// Função para atualizar as trilhas de feromônio com base nas soluções encontradas
void update_pheromone_trails(double trails[][N], Solution best_solution)
{
    int i, j;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            trails[i][j] *= (1.0 - RHO); // Evaporação
        }
    }
    for (i = 0; i < N - 1; i++)
    {
        trails[best_solution.path[i]][best_solution.path[i + 1]] += Q / best_solution.length;
        trails[best_solution.path[i + 1]][best_solution.path[i]] += Q / best_solution.length;
    }
}

// Função principal que executa o algoritmo
void ant_colony_optimization(Node nodes[])
{
    double trails[N][N];
    Solution best_solution;
    int i, j;
    srand(time(NULL)); // Inicializa a semente aleatória
    init_pheromone_trails(trails);
    for (i = 0; i < MAX_ITER; i++)
    {
        Solution solution = construct_solution(trails, nodes);
        if (solution.length < best_solution.length || i == 0)
        {
            best_solution = solution;
        }
        update_pheromone_trails(trails, best_solution);
    }
    printf("Melhor caminho encontrado: ");
    for (i = 0; i < N; i++)
    {
        printf("%d ", best_solution.path[i]);
    }
    printf("\nComprimento do caminho: %lf\n", best_solution.length);
}
// Função main para testar o algoritmo com um conjunto de pontos aleatórios
int main()
{
    Node nodes[N];
    int i;
    srand(time(NULL)); // Inicializa a semente aleatória
    for (i = 0; i < N; i++)
    {
        nodes[i].x = (double)rand() / RAND_MAX; // Coordenada x aleatória entre 0 e 1
        nodes[i].y = (double)rand() / RAND_MAX; // Coordenada y aleatória entre 0 e 1
    }
    ant_colony_optimization(nodes);
    return 0;
}