#!/bin/bash
#Full Run All Algorithms and Functions - furaaf.sh
#Como executar o script:
#No makefile, execute o comando "make furaaf"
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
    new_config=$(echo $config | sed "s/_/ /g")
    parcial_name=$(extract_param "$new_config" "A")
    clear
    for func in $(seq 1 1); do
        rm results/tcc/result_[$parcial_name][f$1].txt
        ./coleta-info.sh -n 20 -c "$new_config" -f $1 -t 1 | tee output-coleta-info[$parcial_name][f$1].dat
        result=$(cat output-coleta-info[$parcial_name][f$1].dat | tail -n 6)
        echo -e $result >> results/tcc/result_[$parcial_name][f$1].txt
        rm output-coleta-info[$parcial_name][f$1].dat
        tput reset
    done;
done;