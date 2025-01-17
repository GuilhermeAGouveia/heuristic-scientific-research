#!/bin/bash

population_size=""
dimension=""
island_size=""
num_epocas=""
num_generations_per_epoca=""
gens_final_epoca=""
function_number=""

function read_parameters_file() {

    #echo "read_parameters_file"
    parameters_filename="$1/_parametros.dat"
    while IFS= read -r line; do
        if [[ $line == *"population_size"* ]]; then
            read -r _ population_size <<<$(echo "${line//,/}")
        fi

        if [[ $line == *"dimension"* ]]; then
            read -r _ dimension <<<$(echo "${line//,/}")
        fi

        if [[ $line == *"island_size"* ]]; then
            read -r _ island_size <<<$(echo "${line//,/}")
        fi

        if [[ $line == *"num_epocas"* ]]; then
            read -r _ num_epocas <<<$(echo "${line//,/}")
        fi

        if [[ $line == *"num_generations"* ]]; then
            read -r _ num_generations_per_epoca <<<$(echo "${line//,/}")
        fi

        if [[ $line == *"gens_final_epoca"* ]]; then
            read -r _ gens_final_epoca <<<$(echo "${line//,/}")
        fi

        if [[ $line == *"function_number"* ]]; then
            read -r _ function_number <<<$(echo "${line//,/}")
        fi
    done <"$parameters_filename"
}

while getopts ":p:n:c:" o; do
    case "${o}" in
    p)
        path_folder=${OPTARG}
        ;;
    n)
        num_instances=${OPTARG}
        ;;
    c)
        config=${OPTARG}
        ;;
    *)
        usage
        ;;
    esac
done

undo_epocas() {

    # Diretório de destino
    dest_dir="$1/$2"
    current_generation=0
    gen=$num_generations_per_epoca
    mkdir -p "$dest_dir"

    for ((i = 0; i < $num_epocas; i++)); do
        if [ "$i" -eq "$(($num_epocas - 1))" ] && [ ! "$gens_final_epoca" -eq 0 ]; then
            gen=$gens_final_epoca
        fi
        for ((j = 0; j < island_size; j++)); do
            for ((k = 0; k < gen; k++)); do
                mkdir -p "$dest_dir/population_$j"
                current_path_gen="$1/epoca_$i/population_$j/generation_$k.log"
                new_path_gen="$dest_dir/population_$j/generation_$current_generation.log"
                mv $current_path_gen $new_path_gen
                #echo "$current_path_gen para $new_path_gen"
                ((current_generation++))
            done
            current_generation=$(($current_generation - $gen))
        done
        current_generation=$(($gen + $current_generation))
        rm -rf "$1/epoca_$i"
    done

    #echo "Gerações movidas"

}

main() {


    if [ -z "$config" ] || [ -z "$path_folder" ] || [ -z $num_instances ]; then
        echo "Metrics_instances Erro: Paramtro(s) vazio(s): Config: $i, Path: $path_folder, Num_instanc:$num_instances" >> erro.txt
    fi
    path_folder_filter=$(echo "$path_folder" | grep -o '_-A_[^ ]*' | head -1)
    echo "config:$config config_fil: $path_folder_filter path: $path_folder  num_instances: $num_instances" >> erro2.txt

    read_parameters_file "$path_folder"

    if [ $gens_final_epoca -eq 0 ] ; then
        total_generations=$(echo "$num_generations_per_epoca * $num_epocas" | bc)
    else
        total_generations=$(echo "$num_generations_per_epoca * ($num_epocas - 1) + $gens_final_epoca" | bc)
    fi

    generations_per_instance=$(echo "$total_generations / $num_instances" | bc)

    if [ $generations_per_instance -eq 0 ] ; then
        generations_per_instance=$total_generations
        num_instances=1
    fi
    
    num_aux=0
    undo_path_name="epoca_0"
    if [ $num_epocas -gt 1 ]; then
        num_aux="1$(date +%M%S%3N)"
        undo_path_name="epoca_$num_aux"
        undo_epocas "$path_folder" "$undo_path_name"
    fi

    gen_init=0
    gen_final=$generations_per_instance
    #echo "$path_folder $num_aux $gen_init $gen_final"
    for ((i = 0; i < $num_instances; i++)); do
        #rm -f "logs_genetica/metrics/$path_folder_filter/epoca_$num_aux/${gen_init}_$((gen_final - 1)).txt" 2>/dev/null
        ./metrics $path_folder $num_aux $gen_init $gen_final || echo "Erro ao executar ./metrics com $path_folder $num_aux $gen_init $gen_final" >> erro.txt &
        gen_init=$gen_final
        if [ $i -eq "$(($num_instances - 2))" ]; then
            gen_final=$total_generations
        else
            gen_final=$(($gen_final + $generations_per_instance))
        fi
    done

    wait

    #path_folder_filter=$(echo "$path_folder" | grep -o '_-A[[:alnum:]]*')
    #path_folder_filter=$(echo "$path_folder" | grep -o '_-A_[^ ]*' | head -1)
    path_folder_filter=$config
    # if [ -z "$path_folder_filter" ]; then
    #     echo "Erro: Não foi possível obter a config $3" >> erro.txt
    #     echo path_folder: $path_folder >> erro.txt
    #     exit 1
    # fi
    path_output="logs_genetica/metrics/$path_folder_filter/$undo_path_name"
    > "$path_output/metrics_F$function_number.txt" || echo "Erro ao criar arquivo $path_output/metrics_F$function_number.txt" >> erro.txt


    gen_init=0
    gen_final=$generations_per_instance
    for ((i = 0; i < $num_instances; i++)); do
        cat "$path_output/$gen_init"_"$(($gen_final - 1)).txt" >> "$path_output/metrics_F$function_number.txt" || echo "Erro ao concatenar arquivos para $path_output/metrics_F$function_number.txt" >> erro.txt
        rm "$path_output/$gen_init"_"$(($gen_final - 1)).txt" || echo "Erro ao remover gen_init_gen_final.txt" >> erro.txt
        gen_init=$gen_final
        if [ $i -eq $(($num_instances - 2)) ]; then
            gen_final=$total_generations
        else
            gen_final=$(($gen_final + $generations_per_instance))
        fi
    done


    mv "$path_output"/* "$path_output"/../../.. || echo "Erro ao mover arquivos de $path_output" >> erro.txt
    rm -rf "$(dirname "$(dirname "$(dirname "$path_output")")")/F_$function_number" || echo "Erro ao remover diretório dirname" >> erro.txt


}

main "$1" "$2"
# echo $population_size
# echo $dimension
# echo $island_size
# echo $num_epocas
# echo $num_generations_per_epoca
# echo $gens_final_epoca
