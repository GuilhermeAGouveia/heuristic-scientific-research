#!/usr/bin/env bash

# Options
n_execucoes=10 # Default value
function_number=3 # Default value
time_limit=10 # Default value

translate_alg_int_to_alg_name() {
    config_alg=$1
    string_result="["
    for i in $(echo $config_alg | tr "," "\n"); do

        case $i in
            0)
                string_result="$string_result PSO"
                ;;
          
            1)
                string_result="$string_result DE"
                ;;
            2)
                string_result="$string_result ACO"
                ;;
            3)
                string_result="$string_result CLONALG"
                ;;
            4)
                string_result="$string_result GA"
                ;;
            *)
                echo "Algoritmo desconhecido ???"
                exit 1
                ;;
        esac
    done;
    string_result="$string_result ]"
    echo -e $string_result
}

usage() { echo "Usage: $0 [-n <Numero de execuções>] [-f <Numero da função de teste de 1 a 15>] [-c <Nome do código evolutivo que será executado>] [-t <Tempo máximo de cada execução>]" 1>&2; exit 1; }

while getopts ":n:f:t:c:" o; do
    case "${o}" in
        n)
            n_execucoes=${OPTARG}
            ;;
        f)
            function_number=${OPTARG}
            ;;
        t)
            time_limit=${OPTARG}
            ;;
        c)
            alg_config=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

# Math

# Function to calculate the mean of an array
# Usage: mean "${array[@]}"
mean() {
    local sum=0
    local count=0
    for i in "$@"; do
        sum=$(echo "$sum + $i" | bc)
        count=$(echo "$count + 1" | bc)
    done
    echo "$sum / $count" | bc -l
}

# Function to calculate the standard deviation of an array
# Usage: std "${array[@]}"
std() {
    local mean=$(mean "$@")
    local sum=0
    local count=0
    for i in "$@"; do
        sum=$(echo "$sum + ($i - $mean)^2" | bc)
        count=$(echo "$count + 1" | bc)
    done
    echo "sqrt($sum / $count)" | bc -l
}

# Main code

clean_line() { printf "\r"; }

mount_progress_bar() {
    local progress=$1
    clean_line
    set_color_progress $progress
    progress-bar $progress
}

set_color_progress() {
    if (( $1 < 25 )); then
        tput setaf 1
    elif (( $1 < 50 )); then
        tput setaf 3
    elif (( $1 < 75 )); then
        tput setaf 3
    else
        tput setaf 4
    fi
}

define_command_evol() {
    echo "./evol -A $alg_config -f $function_number -t $time_limit -k 3" 
}

show_indicator_algorithm() {
    formated_algs="config: $(translate_alg_int_to_alg_name $alg_config)"
    echo -e $formated_algs
}

show_indicator_function() {
    string="funções:"
    for i in $(seq 1 15); do
        if [ $i == $function_number ]; then
            string="$string [$i]"
        else
            string="$string    $i"
        fi
    done;
    echo -e $string
}

main () {
    source libs/progress-bar/progress-bar.sh
    echo -e "Realizando ${n_execucoes} execuções..."
    #echo -e "Código em execução: $alg_path\n"
    show_indicator_algorithm
    show_indicator_function
    tput civis

    resultado=0

    minimo=10000000000
    valor_atual=0
    maximo=0

    array_values=()

    semente=0

    vertical_center=$((($(tput lines) - 3 )/ 2))
    for i in $(seq 1 $vertical_center); do
        printf "\n"
    done


    mount_progress_bar 0 $n_execucoes
    for i in $(seq 1 $n_execucoes); do
        resultado=$(eval $(define_command_evol $alg_config) | tail -n 1)
        
        valor_atual=$(echo $resultado | grep Best | cut -d' ' -f2)
        array_values+=($valor_atual)
        if (( $(echo "$minimo > $valor_atual" | bc -l) )); then
            minimo=$valor_atual
        fi

        if (( $(echo "$maximo < $valor_atual" | bc -l) )); then
            maximo=$valor_atual
        fi

        mount_progress_bar $((i*100/n_execucoes))

    done;
    tput reset
    tput setaf 2
    echo -e "\nResultado para função $function_number:\n"
    echo "Minimo: $minimo"
    echo "Maximo: $maximo"
    echo "Média: $(mean "${array_values[@]}")"
    echo "Desvio padrão: $(std "${array_values[@]}")"
}

main