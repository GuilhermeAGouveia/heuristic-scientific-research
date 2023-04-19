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
#define STATISTICS(x)
#define DEBUG(x)
#define LOG(x) x

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 20 // number of particles
#define D 10 // number of dimensions
#define MAX_ITERATIONS 100 // maximum number of iterations
#define W 0.7 // inertia weight
#define C1 1.5 // cognitive component
#define C2 1.5 // social component
#define V_MAX 1.0 // maximum velocity

double f(double x[D]) {
    // function to optimize
    double sum = 0.0;
    for (int i = 0; i < D; i++) {
        sum += pow(x[i], 2.0);
    }
    return sum;
}

typedef struct {
    individuo position[D];
    double velocity[D];
    double personal_best_position[D];
    double personal_best_value;
} particle;

void init_particle(particle *p) {
    for (int i = 0; i < D; i++) {
        p->position[i] = ((double)rand() / RAND_MAX) * 10.0 - 5.0;
        p->velocity[i] = 0.0;
        p->personal_best_position[i] = p->position[i];
    }
    p->personal_best_value = f(p->position);
}

void update_particle(particle *p, double global_best_position[D]) {
    for (int i = 0; i < D; i++) {
        p->velocity[i] = W * p->velocity[i] + C1 * ((double)rand() / RAND_MAX) * (p->personal_best_position[i] - p->position[i]) + C2 * ((double)rand() / RAND_MAX) * (global_best_position[i] - p->position[i]);
        if (p->velocity[i] > V_MAX) {
            p->velocity[i] = V_MAX;
        } else if (p->velocity[i] < -V_MAX) {
            p->velocity[i] = -V_MAX;
        }
        p->position[i] += p->velocity[i];
    }
    double new_value = f(p->position);
    if (new_value < p->personal_best_value) {
        p->personal_best_value = new_value;
        for (int i = 0; i < D; i++) {
            p->personal_best_position[i] = p->position[i];
        }
    }
}

int main() {
    srand(time(NULL));

    particle particles[N];
    double global_best_position[D];
    double global_best_value = INFINITY;

    for (int i = 0; i < N; i++) {
        init_particle(&particles[i]);
        if (particles[i].personal_best_value < global_best_value) {
            global_best_value = particles[i].personal_best_value;
            for (int j = 0; j < D; j++) {
                global_best_position[j] = particles[i].personal_best_position[j];
            }
        }
    }

    for (int t = 0; t < MAX_ITERATIONS; t++) {
        for (int i = 0; i < N; i++) {
            update_particle(&particles[i], global_best_position);
            if (particles[i].personal_best_value < global_best_value) {
                global_best_value = particles[i].personal_best_value;
                for (int j = 0; j < D; j++) {
                    global_best_position[j] = particles[i].personal_best_position[j];
                }
            }
        }
        printf("Iteration %d: %lf", t, global_best_position);
            for (int i = 1; i < D; i++) {
                printf(", %f", global_best_position[i]);
            }
    printf(") = %f\n", global_best_value);
    }
    return 0;
}
