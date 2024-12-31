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
    sed -i '1d' $file_processes.txt
    echo $current_total_process >> $file_processes.txt

    rm -r $file_processes

}

lock_file="paramsLock"
cores=48
cd /scratch/unifal-mg/simgenetic/iago.carvalho/H3/src

# Tenta obter o lock para acessar params.txt
while ! mkdir "$lock_file" 2>/dev/null; do
    echo "Processo $$ aguardando pelo lock."
    sleep $(echo "scale=3; $(gerar_numero_aleatorio) / 1000" | bc)
done

echo "Processo $$ obteve o lock $(date +"%Y-%m-%d %H:%M:%S.%3N")."

echo "Data e hora atual: $(date +"%Y-%m-%d %H:%M:%S.%3N")"

file_input=params.txt
mkdir -p control_n_process
number=$(echo "scale=0; $(gerar_numero_aleatorio) / 1000 " | bc)
file_processes=control_n_process/params_${number}_$$
echo $cores >> $file_processes.txt

for (( i = 0; i < $cores; i++)); do
    line=$(head -n 1 "$file_input")

    read config numeros <<< "$line"

    #config=$(echo "$config" | sed "s/ /_/g")
    echo $config
    numeros=$(echo "$numeros" | tr '_' ' ' | awk '{$1=$1;print}')

    first_function=$(echo "$numeros" | awk '{print $1}')
    last_function=$(echo "$numeros" | awk '{print $NF}')

    if [ ! -n "$numeros" ]; then
        first_function=1
        last_function=5
    fi

    echo "Config: $config  First: $first_function, Last: $last_function"

    sed -i '1d' "$file_input"
    if [[ ! $config =~ -A ]]; then
        free_core_file $(( $cores - $i ))
        echo "Liberou $(( $cores - $i )) cores"
        break
    else
        ./furaaf_main.sh "$config" $file_processes $cores $first_function $last_function 30 &
       
    fi
done

rm -rf $lock_file
echo "Processo $$ devolveu o lock $(date +"%Y-%m-%d %H:%M:%S.%3N")."

wait

echo  "Script END $(date +"%Y-%m-%d %H:%M:%S.%3N")"


