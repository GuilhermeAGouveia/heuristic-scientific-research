#!/bin/bash
# Full Run All Algorithms and Functions - furaaf.sh
# Como executar o script:
# No makefile, execute o comando "make furaaf"

# Caminho do arquivo de lock

free_core_file() {  #Libera n cores passados por parâmetro
    while ! mkdir "$file_processes" 2>/dev/null; do
    sleep 0.2
    done

    current_total_process=$(head -n 1 "$file_processes.txt")
    current_total_process=$(echo "$current_total_process - $1" | bc)
    sed -i '1d' $file_processes.txt
    echo $current_total_process >> $file_processes.txt

    rm -r $file_processes

}

lock_file="paramsLock"
cores=48

# Tenta obter o lock para acessar params.txt
while ! mkdir "$lock_file" 2>/dev/null; do
    echo "Processo $$ aguardando pelo lock."
    sleep 0.2
done

echo "Processo $$ obteve o lock."

echo "Data e hora atual: $(date +"%Y-%m-%d %H:%M:%S.%3N")"

file_input=params.txt
mkdir -p control_n_process
file_processes=control_n_process/params$$
echo $cores >> $file_processes.txt

for (( i = 0; i < $cores; i++)); do
    config=$(head -n 1 "$file_input" | sed "s/ /_/g")
    sed -i '1d' "$file_input"
    if [[ ! $config =~ -A ]]; then
        free_core_file $(( $cores - $i ))
        echo "Liberou $(( $cores - $i )) cores"
        break
    else
        echo $config
        ./furaaf_main.sh "$config" $file_processes $cores &
       
    fi
done

rm -rf $lock_file

wait

echo  "Script END $(date +"%Y-%m-%d %H:%M:%S.%3N")"


