#!/bin/bash
# Full Run All Algorithms and Functions - furaaf.sh
# Como executar o script:
# No makefile, execute o comando "make furaaf"

# Caminho do arquivo de lock
gerar_numero_aleatorio() {
    # Calcula o máximo (10 minutos em milissegundos)
    max_ms=$((2 * 60 * 1000))
    # Gera um número aleatório entre 0 e $max_ms
    numero_aleatorio=$(shuf -i 0-$max_ms -n 1)
    echo $numero_aleatorio
}


free_core_file() {  #Libera n cores passados por parâmetro
    while ! mkdir "$file_processes" 2>/dev/null; do
    sleep 0.2
    done

    current_total_process=$(head -n 1 "$file_processes.txt")
    current_total_process=$(echo "$current_total_process - $1" | bc)
    echo $current_total_process > $file_processes.txt

    rm -r $file_processes

}

lock_file="paramsLock"
cores=$1
max_simul_instances=$2
n_executions=$3
#cd /scratch/unifal-mg/simgenetic/iago.carvalho/H3

# Tenta obter o lock para acessar params.txt

echo "Data e hora atual: $(date +"%Y-%m-%d %H:%M:%S.%3N")"

file_input=params.txt
mkdir control_n_process
number=$(echo "scale=0; $(gerar_numero_aleatorio) / 1000 " | bc)
file_processes="control_n_process/params_${number}_$$"

#echo $cores >> $file_processes.txt
echo "0" > "${file_processes}TR.txt"
> erro.txt
> erro2.txt

num_instances_to_run=$max_simul_instances
echo $max_simul_instances > "$file_processes.txt"
instances_in_execution=0
run=1

process_current="${file_processes}PD/process_current.txt"
mkdir "${file_processes}PD"
> "$process_current"
> "${file_processes}PD/process_current2.txt"

while :; do
    total_instances_in_file=$(wc -l < "$file_input")
    diff=$(( $max_simul_instances - ($total_instances_in_file + $instances_in_execution)))
    if (($diff > 0));then
        free_core_file $diff
        max_simul_instances=$(($max_simul_instances - $diff))
        num_instances_to_run=$total_instances_in_file
    fi    

    for (( i = 0; i < $num_instances_to_run && "$run" == 1; i++)); do
        while ! mkdir "$lock_file" 2>/dev/null; do
            echo "Processo $$ aguardando pelo lock."
            sleep 0.2
        done

        line=$(head -n 1 "$file_input")
        sed -i '1d' "$file_input"
        rm -rf $lock_file
        if [ -z "$line" ]; then
            run=0
            break
        fi
        read config numeros <<< "$line"

        #config=$(echo "$config" | sed "s/ /_/g")
        echo $config
        numeros=$(echo "$numeros" | tr '_' ' ' | awk '{$1=$1;print}')

        first_function=$(echo "$numeros" | awk '{print $1}')
        last_function=$(echo "$numeros" | awk '{print $NF}')

        if [ -z "$numeros" ]; then
            first_function=1
            last_function=15
        fi

        echo "Config: $config  First: $first_function, Last: $last_function"

        #if [[ ! $config =~ -A ]]; then 
        ./furaaf_main.sh "$config" "$file_processes" "$cores" "$first_function" "$last_function" "$n_executions" &
        wait
        ((total_instances_in_file--))
        ((instances_in_execution++))
    done
    
    #sleep 0.5

    # process_status=$(ps aux --sort=-%cpu | grep -E "metric|dire|coleta|furaaf" | grep -v grep | awk '$3 > 0.1')
    # cpu_usage=$(echo "$process_status" | awk '$11 ~ /dire/ {print $3, $8}' | sort -n | head -n 1)
    # count=$(echo "$process_status" | wc -l)
    # count2=$(ps aux --sort=-%cpu | awk '$3 > 0.01 {count++} END {print count}')
    # if [[ -z "$count" ]]; then
    #     count=0  
    # fi
    # echo "$count $count2 $cpu_usage" >> $process_current


    # ps aux --sort=-%cpu >> "${file_processes}PD/process_current2.txt"


    while ! mkdir "${file_processes}TR" 2>/dev/null; do
        sleep 0.2
    done

    finished_instances=$(head -n 1 "${file_processes}TR.txt") 

    instances_in_execution=$(($instances_in_execution - $finished_instances))
    num_instances_to_run=$finished_instances
    echo "0" > "${file_processes}TR.txt"
    rm -rf "${file_processes}TR"

    if [ $total_instances_in_file -eq 0 ] && [ $instances_in_execution -eq 0 ]; then
        break
    fi

done
echo "wait"
wait

free_core_file $max_simul_instances

echo  "Script END $(date +"%Y-%m-%d %H:%M:%S.%3N")"


