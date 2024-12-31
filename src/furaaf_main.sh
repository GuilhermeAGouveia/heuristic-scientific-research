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

return_free_cores() {   #Retorna o total de cores livres
    
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

free_core_file() {  #Libera n cores passados por parâmetro
    while ! mkdir "$total_process_file" 2>/dev/null; do
    sleep 0.2
    done

    current_total_process=$(head -n 1 "$total_process_file.txt")
    current_total_process=$(echo "$current_total_process - $1" | bc)
    sed -i '1d' $total_process_file.txt
    echo $current_total_process >> $total_process_file.txt

    rm -r $total_process_file

}

run_function() { 
    func=$1
    echo ""
    echo "$config funcao $func"



    # result=$(./coleta-info.sh -n $n_execucoes -c "$new_config" -f $func -t 10 -Z $temporary_folder | tee output-coleta-info[$parcial_name][f$func].dat)
    resultado_coleta=$(./coleta-info.sh -n $n_execucoes -c "$new_config" -f $func -t 10 -F $total_process_file -C $cores)
    arquivo_saida="output-coleta-info[$parcial_name][f$func].dat"
    echo "$resultado_coleta" > $arquivo_saida

    result=$(cat output-coleta-info[$parcial_name][f$func].dat | tail -n 6)
    mkdir logs_genetica/coleta_info/$config 2>/dev/null
    rm logs_genetica/coleta_info/$config/[f$func].txt 2>/dev/null
    echo -e $result >>logs_genetica/coleta_info/$config/[f$func].txt
    rm output-coleta-info[$parcial_name][f$func].dat 2>/dev/null
    
    ./metrics_all $path_metrics $func $n_execucoes &
    wait
    cores_to_remove=1               #Função atual

    free_core_file $cores_to_remove
}

n_execucoes=$6
first_function=$4
last_function=$5
config=$1

total_process_file=$2
cores=$3
current_function=$first_function

function_n_cores=1

new_config=$(echo $config | sed "s/_/ /g")
parcial_name=$(extract_param "$new_config" "A")
#rm -rf logs_genetica/metrics/$config
#rm -rf logs_genetica/coleta_info/$config
path_metrics="logs_genetica/metrics/$config"

while [ $current_function -le $last_function ]; do

     while [  $function_n_cores -eq 0 ]; do
         function_n_cores=$(echo "$(return_free_cores)")
     done

    if [ $(echo "$function_n_cores + $current_function - 1" | bc) -gt $last_function ]; then
        free_cores_to_function=$(( $last_function - $current_function + 1 ))  #Verifica quantos cores são necessários
        return_cores=$(echo "($function_n_cores - $free_cores_to_function)" | bc) #Quantos cores devem ser liberados
        function_n_cores=$free_cores_to_function
        free_core_file $return_cores
    fi

    echo ""
    echo "-----------------------------------------------------------------------------"
    echo $config
    echo "Valor de function_n_cores: $function_n_cores"
    echo ""

    for ((i = 0; i < $function_n_cores; i++));do
        run_function $current_function  &
        current_function=$(($current_function + 1))
        sleep 0.1
    done

    wait

    function_n_cores=0
    
done


rm -rf $path_metrics/exe* 
echo $config END  $(date +"%Y-%m-%d %H:%M:%S.%3N")


# for ((function = $first_function; function <= $last_function; function++)); do
#     ./metrics_all $path_metrics $function $n_execucoes &
# done





