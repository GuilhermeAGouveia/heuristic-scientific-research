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
make direct # compila o algoritmo de ilha por escolha direta
all_params=$(cat params.txt | sed "s/ /_/g")
for config in $all_params; do
    new_config=$(echo $config | sed "s/_/ /g")
    parcial_name=$(extract_param "$new_config" "A")
    clear
    for func in $(seq 1 15); do
        rm results/tcc/result_[$parcial_name][f$func].txt
        ./coleta-info.sh -n 20 -c "$new_config" -f $func -t 10 | tee output-coleta-info[$parcial_name][f$func].dat
        result=$(cat output-coleta-info[$parcial_name][f$func].dat | tail -n 6)
        echo -e $result >> results/tcc/result_[$parcial_name][f$func].txt
        rm output-coleta-info[$parcial_name][f$func].dat
        tput reset
    done;
done;