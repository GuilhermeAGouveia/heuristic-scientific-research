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

# contar_instancias() {
#     pgrep -fl "furaaf.sh" | grep -v grep | wc -l
# }

contar_instancias() {
    pgrep -f "furaaf.sh" | grep -v "^$$\$" | wc -l
}



#rm -rf logs_genetica/furaaf/*
#rm -rf logs_genetica/coleta_info/*

echo "Executando o make"
make direct # compila o algoritmo de ilha por escolha direta
n_execucoes=30
first_function=$1
last_function=$2
all_params=$(cat params.txt | sed "s/ /_/g")
for config in $all_params; do
    new_config=$(echo $config | sed "s/_/ /g")
    parcial_name=$(extract_param "$new_config" "A")
    #rm -rf logs_genetica/metrics/$config
    #rm -rf logs_genetica/coleta_info/$config
    path_metrics="logs_genetica/metrics/$config"
    clear
    for func in $(seq $first_function $last_function); do
        #rm results/tcc/result_[$parcial_name][f$func].txt
        temporary_folder=$(date +%H%M%S_%3N)$config

        ./coleta-info.sh -n $n_execucoes -c "$new_config" -f $func -t 10 -Z $temporary_folder | tee output-coleta-info[$parcial_name][f$func].dat
        result=$(cat output-coleta-info[$parcial_name][f$func].dat | tail -n 6)
        mkdir logs_genetica/coleta_info/$config
        rm logs_genetica/coleta_info/$config/[f$func].txt
        echo -e $result >>logs_genetica/coleta_info/$config/[f$func].txt
        rm output-coleta-info[$parcial_name][f$func].dat
        tput reset

        ./metrics_all $path_metrics $func $n_execucoes &
        wait
    done

    # for ((function = $first_function; function <= $last_function; function++)); do
    #     ./metrics_all $path_metrics $function $n_execucoes &
    # done

    wait

    tput reset

done
num_instances="$(contar_instancias)"
# echo $num_instances
if [ "$num_instances" -eq 1 ]; then
    shutdown -h 5
fi
