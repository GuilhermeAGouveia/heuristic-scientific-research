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
make  # compila o algoritmo de ilha por escolha direta
n_execucoes=30
first_function=$1
last_function=$2
all_params=$(cat params.txt | sed "s/ /_/g")
for config in $all_params; do
       ./furaaf_main.sh $config &
       sleep 0.2
    
done
wait
tput reset
num_instances="$(contar_instancias)"
# echo $num_instances
# if [ "$num_instances" -eq 1 ]; then
#     shutdown -h 5
# fi
