#!/bin/bash
#Full Run All Algorithms and Functions - furaaf.sh

# Coloque _ no inicio do nome dos algoritmos que não deseja executar
# Exemplo: _nome_do_algoritmo.sh

extract_param() {
    local params=$1
    local param=$2
    echo $params | cut -d"$param" -f 2 | cut -d' ' -f 2
}

echo "Executando o make"
export COD_NAME="algoritmoIlhaEscolhaDireta"
make
all_params=$(cat params.txt | sed "s/ /_/g")
for config in $all_params; do

    rm results/tcc/result_[$config].txt
    new_config=$(echo $config | sed "s/_/ /g")
    clear
    parcial_name=$(extract_param "$new_config" "A")
    for func in $(seq 1 15); do
        ./coleta-info.sh -n 20 -c "$new_config" -f $func -t 1 | tee output-coleta-info.dat
        result=$(cat output-coleta-info.dat | tail -n 6)
        echo -e $result >> results/tcc/result_[$parcial_name].txt
        rm output-coleta-info.dat
        tput reset
    done;
done;