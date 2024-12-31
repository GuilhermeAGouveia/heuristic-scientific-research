#!/usr/bin/env bash

# Options
n_execucoes=10    # Default value
function_number=3 # Default value
time_limit=10     # Default value
n_cores=1

translate_alg_int_to_alg_name() {
    config_alg=$1
    string_result="["
    count=0
    for i in $(echo $config_alg | tr "," "\n"); do
        for j in $(seq 1 $i); do
            case $count in
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
        done
        count=$((count + 1))
    done
    string_result="$string_result ]"
    echo -e $string_result
}

usage() {
    echo "Usage: $0 [-n <Numero de execuções>] [-f <Numero da função de teste de 1 a 15>] [-c <Nome do código evolutivo que será executado>] [-t <Tempo máximo de cada execução>]" 1>&2
    exit 1
}

while getopts ":n:f:t:c:Z:F:C:" o; do
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
    Z)
        temporary_folder=${OPTARG}
        ;;
    F)
        total_process_file=${OPTARG}
        ;;
    C)
        cores=${OPTARG}
        ;;
    *)
        usage
        ;;
    esac
done
shift $((OPTIND - 1))

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
    if (($1 < 25)); then
        tput setaf 1
    elif (($1 < 50)); then
        tput setaf 3
    elif (($1 < 75)); then
        tput setaf 3
    else
        tput setaf 4
    fi
}

extract_param() {
    local params=$1
    local param=$2
    echo $params | cut -d"$param" -f 2 | cut -d' ' -f 2
}

define_command_evol() {
    echo "./dire $alg_config -f $function_number -t $time_limit -Z $temporary_folder"
}

show_indicator_algorithm() {
    param_A=$(extract_param "$alg_config" "A")
    formated_algs="config: $(translate_alg_int_to_alg_name $param_A)"
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
    done
    echo -e $string
}

show_command_exec() {
    echo -e "Executando: $(define_command_evol)"
}

move_arquivos() {
    origem="$1"
    destino="$2"
    nova_pasta="$3"

    if [ -z "$nova_pasta" ]; then
        mv "$origem"/* "$destino"
    else
        mkdir -p "$destino/$nova_pasta" 2>/dev/null
        mv "$origem"/* "$destino/$nova_pasta"
    fi
}

return_free_cores() {
    
    while ! mkdir "$total_process_file" 2>/dev/null; do
        sleep 0.2
    done

    current_total_process=$(head -n 1 "$total_process_file.txt")
    free_cores=$(echo "$cores - $current_total_process" | bc)
    sed -i '1d' "$total_process_file.txt"
    echo "$cores" >> "$total_process_file.txt"
    rm -rf "$total_process_file"
    echo $free_cores  # Use echo para retornar o valor
}

checks_stopped_processes() {
    folder_name=SP
    folder_name="$total_process_file$folder_name"
    run=1
    while (( $run != 0 )); do
        while ! mkdir "$folder_name" 2>/dev/null; do
            sleep 0.2
        done

        if [ ! -e "$folder_name.txt" ]; then
            echo 0 > "$folder_name.txt"  
        fi

        current_value=$(head -n 1 "$folder_name.txt")

        if (( $current_value == 0 )); then
            sed -i '1d' "$folder_name.txt"
            echo 1 >> "$folder_name.txt"
            run=0
            rm -rf "$folder_name"
            echo "$alg_config LIBERADO" >> GG.txt

        else
            rm -rf "$folder_name"
            echo "$alg_config PARADO" >> GG.txt
            sleep 60
        fi
    done
}

stopped_processes_free() {
    folder_name=SP
    folder_name="$total_process_file$folder_name"

    while ! mkdir "$folder_name" 2>/dev/null; do
        sleep 0.2
    done

    sed -i '1d' "$folder_name.txt"
    echo 0 >> "$folder_name.txt"

    rm -rf "$folder_name"

}


return_one_free_core() {
    run=1
    free=0
    while (( $run != 0 )); do
        while ! mkdir "$total_process_file" 2>/dev/null; do
            sleep 0.2
        done

        current_total_process=$(head -n 1 "$total_process_file.txt")
        free_cores=$(echo "$cores - $current_total_process" | bc)

        if (( $free_cores >= 2 )); then
            sed -i '1d' "$total_process_file.txt"
            echo $(( $current_total_process + 2 )) >> "$total_process_file.txt"
            free=$(( $free + 2 )) 
        fi

        if (( $free_cores == 1 )); then
            sed -i '1d' "$total_process_file.txt"
            echo $(( $current_total_process + 1 )) >> "$total_process_file.txt"
            free=$(( $free + 1 )) 
        fi

        if (( $free_cores == 0 )); then
            run=0
        fi
        rm -rf "$total_process_file"
        sleep 0.4
    done
    echo $free

}


free_core_file() {
    while ! mkdir "$total_process_file" 2>/dev/null; do
    sleep 0.2
    done

    current_total_process=$(head -n 1 "$total_process_file.txt")
    current_total_process=$(echo "$current_total_process - $1" | bc)
    sed -i '1d' $total_process_file.txt
    echo $current_total_process >> $total_process_file.txt

    rm -r $total_process_file

}


main() {
    source libs/progress-bar/progress-bar.sh
    echo -e "Realizando ${n_execucoes} execuções..."
    echo -e "Código em execução: $alg_path\n"
    show_indicator_algorithm
    show_indicator_function
    show_command_exec
    tput civis

    resultado=0

    minimo=10000000000
    valor_atual=0
    maximo=0

    array_values=()

    semente=0

    vertical_center=$((($(tput lines) - 3) / 2))
    for i in $(seq 1 $vertical_center); do
        printf "\n"
    done

    mount_progress_bar 0 $n_execucoes
    path_data="logs_genetica/furaaf"
    temporary_folder_two="1$(date +%H%M%S_%3N)$temporary_folder"
    mkdir -p $path_data/$temporary_folder_two 2>/dev/null
    for i in $(seq 1 $n_execucoes); do
        resultado=$(eval $(define_command_evol $alg_config) | tail -n 1)

        valor_atual=$(echo $resultado | grep Best | cut -d' ' -f2)
        array_values+=($valor_atual)
        if (($(echo "$minimo > $valor_atual" | bc -l))); then
            minimo=$valor_atual
        fi

        if (($(echo "$maximo < $valor_atual" | bc -l))); then
            maximo=$valor_atual
        fi

        mount_progress_bar $((i * 100 / n_execucoes))

        nova_pasta="execucao_${i}/F_$function_number"
        move_arquivos "$path_data/$temporary_folder" "$path_data/$temporary_folder_two" $nova_pasta
        echo "$path_data/$temporary_folder_two/$nova_pasta"
        
        free_cores=$(echo "$(return_one_free_core)")
        n_cores=$((1 + $free_cores))
        
        alg_aux=$(echo "$alg_config" | sed 's/ -/_-/g' | sed 's/ /_/g')
        a_values=$(echo "$alg_aux" | grep -oP '(?<=-A_)[^_]+')
        p_value=$(echo "$alg_aux" | grep -oP '(?<=-p_)[^_]+')
        min_cores=3


        for value in $(echo "$a_values" | tr ',' ' '); do
            if (( value >= 25 && $p_value >= 250 && $n_cores < $min_cores )); then
                free_core_file $n_cores
                n_cores=0
                checks_stopped_processes
                while (( $n_cores < $min_cores )); do
                    free_cores=$(echo "$(return_one_free_core)")
                    n_cores=$(($n_cores + $free_cores))
                done
                stopped_processes_free
                break
            fi
        done


        echo "$alg_config $n_cores" >> GG.txt
        ./metrics_instances.sh $path_data/$temporary_folder_two/$nova_pasta/data $n_cores
        wait
        echo "$alg_config FIM" >> GG.txt
        free_core_file $(($n_cores - 1))

        if [ $i -eq 1 ]; then
           cp  $path_data/$temporary_folder_two/$nova_pasta/data/_parametros.dat logs_genetica/metrics/_$(echo $alg_config | sed "s/ /_/g")/_parametros_F$function_number.dat
        fi
        rm -rf $path_data/$temporary_folder_two/* 2>/dev/null
    done
    rm -rf $path_data/$temporary_folder_two 2>/dev/null
    rm -rf $path_data/$temporary_folder 2>/dev/null


    echo -e "\nResultado para função $function_number:\n"
    echo "Minimo: $minimo"
    echo "Maximo: $maximo"
    echo "Média: $(mean "${array_values[@]}")"
    echo "Desvio padrão: $(std "${array_values[@]}")"

}

main
