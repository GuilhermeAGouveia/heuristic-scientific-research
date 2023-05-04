codname = ant_3

compil:;
	gcc -o evol $(codname).c ./libs/statistics.c ./libs/log.c ./libs/crossover.c ./libs/types.c ./libs/utils.c ./libs/funcoes_cec_2015/cec15_test_func.c ./libs/funcoes_cec_2015/ecr_iml.c ./libs/funcoes_cec_2015/evaluation_count_iml.c ./libs/funcoes_cec_2015/record_result_iml.c ./libs/funcoes_cec_2015/rp_iml.c -lm; \

run: compil;
	./evol -f 3 -i 1 -p 5; \

prepare:;
	chmod +x -R ./scripts/*; \

statistics: compil; \
	rm -rf ./statistics/*.dat; \
	./evol -i 1 -f 3 -p 4
	./scripts/sh/gerar_todas_estatisticas.sh; \
