#!/bin/bash
#FUll Run All Algorithms and Functions - furaaf.sh

all_algorithms=$(ls algorithms/* | cut -d'/' -f 2 | cut -d'.' -f1)
for alg in $all_algorithms; do
    rm result_$alg.txt
    alg_path_seed_command="algorithms\/$alg"
    sed "s/<codname>/$alg_path_seed_command/g" makefile_ > makefile
    make
    clear
    for func in $(seq 1 15); do
        ./coleta-info.sh -n 20 -c $alg -f $func | tee output-coleta-info.dat
        result=$(cat output-coleta-info.dat | tail -n 6)
        echo -e $result >> result_$alg.txt
        rm output-coleta-info.dat
        tput reset
    done;
done;