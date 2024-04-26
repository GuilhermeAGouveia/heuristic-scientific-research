#!/bin/bash
#Full Run All Algorithms and Functions - furaaf.sh
#Como executar o script:
#No makefile, execute o comando "make furaaf"
extract_param() {
    local params=$1
    local param=$2
    echo $params | cut -d"$param" -f 2 | cut -d' ' -f 2
}

move_arquivos() {
    origem="$1"
    destino="$2"
    nova_pasta="$3"

    mkdir -p "$destino/$nova_pasta"
    mv "$origem"/* "$destino/$nova_pasta"
}

rm -rf logs_genetica/furaaf/*
rm -rf logs_genetica/coleta_info/*
rm -rf logs_genetica/furaaf/current_logs*

echo "Executando o make"
make direct # compila o algoritmo de ilha por escolha direta
all_params=$(cat params.txt | sed "s/ /_/g")
for config in $all_params; do
    new_config=$(echo $config | sed "s/_/ /g")
    parcial_name=$(extract_param "$new_config" "A")
    clear
    for func in $(seq 1 15); do
        #rm results/tcc/result_[$parcial_name][f$func].txt
        ./coleta-info.sh -n 3 -c "$new_config" -f $func -t 10 | tee output-coleta-info[$parcial_name][f$func].dat
        result=$(cat output-coleta-info[$parcial_name][f$func].dat | tail -n 6)
        mkdir logs_genetica/coleta_info/$config
        echo -e $result >> logs_genetica/coleta_info/$config/[$parcial_name][f$func].txt
        move_arquivos logs_genetica/furaaf/current_logs logs_genetica/furaaf/$config /[$parcial_name][f$func]
        rm output-coleta-info[$parcial_name][f$func].dat
        tput reset
    done;
done;