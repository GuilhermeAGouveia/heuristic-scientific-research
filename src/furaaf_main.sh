#!/bin/bash
# Full Run All Algorithms and Functions - furaaf.sh
# Como executar o script:
# No makefile, execute o comando "make furaaf"

extract_param() {
    local params=$1
    local param=$2
    echo $params | cut -d"$param" -f 2 | cut -d' ' -f 2
}

move_arquivos() {
    origem="$1"
    destino="$2"
    nova_pasta="$3"

    mkdir -p "$destino/$nova_pasta" 2>/dev/null
    mv "$origem"/* "$destino/$nova_pasta"
}

# contar_instancias() {
#     pgrep -fl "furaaf.sh" | grep -v grep | wc -l
# }

contar_instancias() {
    pgrep -f "furaaf.sh" | grep -v "^$$\$" | wc -l
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

n_execucoes=2
first_function=1
last_function=4
config=$1
total_process_file=$2
cores=$3
current_n_cores=1

new_config=$(echo $config | sed "s/_/ /g")
parcial_name=$(extract_param "$new_config" "A")
#rm -rf logs_genetica/metrics/$config
#rm -rf logs_genetica/coleta_info/$config
path_metrics="logs_genetica/metrics/$config"

for func in $(seq $first_function $last_function); do
    echo ""
    echo "$config funcao $func"
    #rm results/tcc/result_[$parcial_name][f$func].txt
    #current_n_cores=$(( $(return_free_cores) + current_n_cores ))
    #free_cores=$(echo "$(return_free_cores)" | grep -oE '^-?[0-9]+(\.[0-9]+)?$')
    free_cores=$(echo "$(return_free_cores)")
    current_n_cores=$(echo "$free_cores + $current_n_cores" | bc)

    echo "Cores $current_n_cores $config"

    temporary_folder=$(date +%H%M%S_%3N)$config

    # result=$(./coleta-info.sh -n $n_execucoes -c "$new_config" -f $func -t 10 -Z $temporary_folder | tee output-coleta-info[$parcial_name][f$func].dat)
    resultado_coleta=$(./coleta-info.sh -n $n_execucoes -c "$new_config" -f $func -t 10 -Z $temporary_folder -C $current_n_cores)
    arquivo_saida="output-coleta-info[$parcial_name][f$func].dat"
    echo "$resultado_coleta" > $arquivo_saida

    result=$(cat output-coleta-info[$parcial_name][f$func].dat | tail -n 6)
    mkdir logs_genetica/coleta_info/$config 2>/dev/null
    rm logs_genetica/coleta_info/$config/[f$func].txt 2>/dev/null
    echo -e $result >>logs_genetica/coleta_info/$config/[f$func].txt
    rm output-coleta-info[$parcial_name][f$func].dat 2>/dev/null

    ./metrics_all $path_metrics $func $n_execucoes &
    wait
done


while ! mkdir "$total_process_file" 2>/dev/null; do
    echo "Processo $config aguardando pelo acesso a $total_process_file."
    sleep 0.2
done
echo "Processo $config obteve acesso em END $(date +"%Y-%m-%d %H:%M:%S.%3N")."

current_total_process=$(head -n 1 "$total_process_file.txt")
current_total_process=$(echo "$current_total_process - $current_n_cores" | bc)
sed -i '1d' $total_process_file.txt
echo $current_total_process >> $total_process_file.txt

rm -r $total_process_file
rm -rf $path_metrics/exe* 




# for ((function = $first_function; function <= $last_function; function++)); do
#     ./metrics_all $path_metrics $function $n_execucoes &
# done





