#!/bin/bash
#FUll Run All Algorithms and Functions - furaaf.sh

# Coloque _ no inicio do nome dos algoritmos que não deseja executar
# Exemplo: _nome_do_algoritmo.sh
gcc utils/combinationNoOrder.c -o combinationNoOrder
all_configurations=$(./combinationNoOrder 4 | tee)
for config in $all_configurations; do
    rm result_[$config].txt
    make
    clear
    for func in $(seq 1 15); do
        ./coleta-info.sh -n 20 -c $config -f $func | tee output-coleta-info.dat
        result=$(cat output-coleta-info.dat | tail -n 6)
        echo -e $result >> result_[$config].txt
        rm output-coleta-info.dat
        tput reset
    done;
done;