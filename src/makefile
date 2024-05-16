commom_libs = ./libs/statistics.c ./libs/log.c ./libs/crossover.c ./libs/types.c ./libs/utils.c ./libs/funcoes_cec_2015/cec15_test_func.c ./libs/funcoes_cec_2015/ecr_iml.c ./libs/funcoes_cec_2015/evaluation_count_iml.c ./libs/funcoes_cec_2015/record_result_iml.c ./libs/funcoes_cec_2015/rp_iml.c ./algorithms/genetic.c ./algorithms/pso.c ./libs/parameters.c ./libs/commom.c ./algorithms/clonalg.c ./algorithms/aco.c ./algorithms/diferencial.c -lm
ilha_proporcional_output = prop
ilha_direta_output = dire
medidas_geneticas_output = metrics
metrics_all_output = metrics_all

all: genetic_metrics genetic_metrics_all direct proportional; \

run: compil;
	./evol; \

prepare:;
	chmod +x -R ./scripts/*; \

statistics: compil; \
	rm -rf ./statistics/*.dat; \
	./evol
	./scripts/sh/gerar_todas_estatisticas.sh; \

genetic_metrics: ;
	gcc -o $(medidas_geneticas_output) -g utils/medidas_geneticas.c $(commom_libs); \

genetic_metrics_all: ;
	gcc -o $(metrics_all_output) -g utils/metrics_all_executations.c $(commom_libs); \

direct: ;
	gcc -o $(ilha_direta_output) -g algoritmoIlhaEscolhaDireta.c $(commom_libs); \

proportional: ;
	gcc -o $(ilha_proporcional_output) -g algoritmoIlhaEscolhaProporcional.c $(commom_libs); \

furaaf: prepare;
	./scripts/sh/verificar_params.sh; \
	./furaaf.sh; \