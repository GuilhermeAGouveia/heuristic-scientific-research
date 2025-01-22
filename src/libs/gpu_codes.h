// gpu_code.h
#ifndef GPU_CODES_H
#define GPU_CODE_H

#ifdef __cplusplus
extern "C" {
#endif

double densityPopulation(populacao **populations, int island_number);
double densityWorld(populacao **populations, int island_number);

#ifdef __cplusplus
}
#endif

#endif // GPU_CODE_H
