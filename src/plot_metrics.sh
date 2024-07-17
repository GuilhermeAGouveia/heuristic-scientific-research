#!/bin/bash

#Arquivos que serão unidos
file1="pergunta_1/GA/teste_2/metrics/_-A_0,0,0,0,2_-p_4_-K_1_-k_0/metrics_all_executions"

file2="pergunta_1/GA/teste_2/metrics/_-A_0,0,0,0,2_-p_4_-K_1_-k_1/metrics_all_executions"

./filter_plot $file1 $file2


#Variáveis
num_func=14
question="pergunta_1"
comparison="A_2/p_4_x_p_4"
columns=2
nomes=("0 " "1 ")

base_path="logs_genetica/perguntas_artigo"
METRICS=("P_Average" "P_SD" "World_Average" "World_SD")

#Obtem os dados unidos
./filter_plot $file1 $file2
output_plot_path="logs_genetica/output_plot"
destination_data="$base_path/$question/GA/plot_metrics/$comparison/joined_metrics"
for m in "${METRICS[@]}";do
   mkdir -p $destination_data/$m
   rm -r $destination_data/$m/*
   mv $output_plot_path/$m/* $destination_data/$m
done

#Plota os gráficos
for ((i = 1; i <= num_func; i++)); do
    for m in "${METRICS[@]}"; do
        func="F$i"
        data_path="$base_path/$question/GA/plot_metrics/$comparison/joined_metrics/$m/${m}_plot_${func}.csv"
        output_path="$base_path/$question/GA/plot_metrics/$comparison/plots/$m/${m}_${func}.png"



        IFS=$'  '
        export NOMES_COLUNAS=$(echo "${nomes[*]}")

        gnuplot -e "filename='$data_path'; outputname='$output_path';yyLabel='$m'; columns='$columns'; nomes_colunas='$NOMES_COLUNAS;" plot_metrics_csv.gp
    done
done

#./filter_plot pergunta_1/GA/teste_2/metrics/_-A_0,0,0,0,2_-p_4_-K_1_-k_0/metrics_all_executions pergunta_1/GA/teste_2/metrics/_-A_0,0,0,0,2_-p_4_-K_1_-k_1/metrics_all_executions pergunta_3/GA/teste_2/metrics/_-A_0,0,0,0,2_-p_40_-K_1_-k_20/metrics_all_executions
#find ./ -type f -name "*.png" -exec rm -f {} +
