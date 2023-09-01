#!/bin/bash
#Full Run All Algorithms and Functions - furaaf.sh

# Coloque _ no inicio do nome dos algoritmos que não deseja executar
# Exemplo: _nome_do_algoritmo.sh
./inputForFuraaf.sh 
echo "Executando o make"
export COD_NAME="algoritmoIlhaEscolhaDireta"
make
all_configurations=$(cat ilhas.txt)
for config in $all_configurations; do
    rm results/tcc/result_[$config].txt
    clear
    for func in $(seq 1 15); do
        ./coleta-info.sh -n 20 -c $config -f $func -t 1 | tee output-coleta-info.dat
        result=$(cat output-coleta-info.dat | tail -n 6)
        echo -e $result >> results/tcc/result_[$config].txt
        rm output-coleta-info.dat
        tput reset
    done;
done;