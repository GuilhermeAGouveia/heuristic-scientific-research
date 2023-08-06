codname = main

compil:;
	gcc -o evol $(codname).c ./libs/statistics.c ./libs/log.c ./libs/crossover.c ./libs/types.c ./libs/utils.c ./libs/funcoes_cec_2015/cec15_test_func.c ./libs/funcoes_cec_2015/ecr_iml.c ./libs/funcoes_cec_2015/evaluation_count_iml.c ./libs/funcoes_cec_2015/record_result_iml.c ./libs/funcoes_cec_2015/rp_iml.c ./algorithms/genetic.c ./algorithms/pso.c ./algorithms/parameters.c ./algorithms/commom.c ./algorithms/clonalg.c ./algorithms/aco.c ./algorithms/diferencial.c -lm; \
	gcc utils/combinationNoOrder.c -o combinationNoOrder; \

run: compil;
	./evol; \

prepare:;
	chmod +x -R ./scripts/*; \

statistics: compil; \
	rm -rf ./statistics/*.dat; \
	./evol
	./scripts/sh/gerar_todas_estatisticas.sh; \
