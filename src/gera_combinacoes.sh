#!/bin/bash

# Valores dos parâmetros
K_VALUES=(1 2 3 4 5 10 20)
k_VALUES=(0.01 0.05 0.1 0.15 0.25 0.5)
P_VALUES=(5 10 25 50 100 250 500 1000)
#A_VALUES=(
#    "2,0,0,0,0" "0,2,0,0,0" "0,0,2,0,0" "0,0,0,2,0" "0,0,0,0,2"
#    "5,0,0,0,0" "0,5,0,0,0" "0,0,5,0,0" "0,0,0,5,0" "0,0,0,0,5"
#    "10,0,0,0,0" "0,10,0,0,0" "0,0,10,0,0" "0,0,0,10,0" "0,0,0,0,10"
#    "25,0,0,0,0" "0,25,0,0,0" "0,0,25,0,0" "0,0,0,25,0" "0,0,0,0,25"
#    "50,0,0,0,0" "0,50,0,0,0" "0,0,50,0,0" "0,0,0,50,0" "0,0,0,0,50")

A_VALUES=(
    "1,0,0,0,0" "0,1,0,0,0" "0,0,1,0,0" "0,0,0,1,0" "0,0,0,0,1")

declare -A matriz
matriz[0, 0]="2,0,0,0,0"
matriz[0, 1]="5,0,0,0,0"
matriz[0, 2]="10,0,0,0,0"
matriz[0, 3]="25,0,0,0,0"
matriz[0, 4]="50,0,0,0,0"

matriz[1, 0]="0,2,0,0,0"
matriz[1, 1]="0,5,0,0,0"
matriz[1, 2]="0,0,10,0,0"
matriz[1, 3]="0,25,0,0,0"
matriz[1, 4]="0,50,0,0,0"

matriz[2, 0]="0,0,2,0,0"
matriz[2, 1]="0,0,5,0,0"
matriz[2, 2]="0,0,10,0,0"
matriz[2, 3]="0,0,25,0,0"
matriz[2, 4]="0,0,50,0,0"

matriz[3, 0]="0,0,0,2,0"
matriz[3, 1]="0,0,0,5,0"
matriz[3, 2]="0,0,0,10,0"
matriz[3, 3]="0,0,0,25,0"
matriz[3, 4]="0,0,0,50,0"

matriz[4, 0]="0,0,0,0,2"
matriz[4, 1]="0,0,0,0,5"
matriz[4, 2]="0,0,0,0,10"
matriz[4, 3]="0,0,0,0,25"
matriz[4, 4]="0,0,0,0,50"

ALG=("PSO" "DE" "ACO" "CLONAL" "GA")

function arredondar() {
    echo "scale=0;(($1 + 0.5)/1)" | bc
}
rm -rf parameters_combinations/teste_3/*

for ((i = 0; i < 5; i++)); do
    mkdir parameters_combinations/teste_3/${ALG[$i]}
    for ((j = 0; j < 5; j++)); do
        for P in "${P_VALUES[@]}"; do
            for K in "${K_VALUES[@]}"; do
                for k in "${k_VALUES[@]}"; do
                    PK=$(echo "$P * $k" | bc)
                    k_INT=$(arredondar $PK)
                    if [ "$k_INT" -eq 0 ]; then
                        k_INT=1
                    fi
                    echo " -A ${matriz[$i, $j]} -p $P -K $K -k $k_INT" >>parameters_combinations/teste_3/${ALG[$i]}/${matriz[$i, $j]}.txt
                done
            done
        done
    done
done

rm -rf parameters_combinations/teste_1/*

for((i = 0; i < 5; i++)); do 
 mkdir parameters_combinations/teste_1/${ALG[$i]}
    for P in "${P_VALUES[@]}"; do
        echo " -A ${A_VALUES[$i]} -p $P -K 1000 -k 0" >>parameters_combinations/teste_1/${ALG[$i]}/${A_VALUES[$i]}.txt
    done
done

echo "Combinacoes geradas com sucesso!"
