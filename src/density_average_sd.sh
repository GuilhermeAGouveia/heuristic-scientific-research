#!/bin/bash

folderPath=$1
function=$2
num_executions=$3

main() {

    arrayD_World=()
    arrayD_Pop=()
    current_gen=0
    gen=9999999

    #obtem a soma dos valores de cada geração
    for ((i = 1; i <= num_executions; i++)); do

        path="$folderPath/execucao_$i/metrics_F$function.txt"
        while IFS= read -r line; do
            read -a densities <<<"$line"
            if [ $i -eq 1 ]; then
                arrayD_Pop[$current_gen]=${densities[0]}
                arrayD_World[$current_gen]=${densities[1]}
            else
                if [ -n "${arrayD_World[$current_gen]}" ]; then
                    arrayD_Pop[$current_gen]=$(bc <<<"scale=6; ${arrayD_Pop[$current_gen]} + ${densities[0]}")
                    arrayD_World[$current_gen]=$(bc <<<"scale=6; ${arrayD_World[$current_gen]} + ${densities[1]}")
                fi
            fi
            ((current_gen++))

        done <"$path"

        if [ $current_gen -lt $gen ]; then
            gen=$current_gen
        fi
        current_gen=0
    done

    #obtem a média
    for ((i = 0; i < $gen; i++)); do
        arrayD_Pop[$i]=$(bc <<<"scale=6; ${arrayD_Pop[$i]} / $num_executions")
        arrayD_World[$i]=$(bc <<<"scale=6; ${arrayD_World[$i]} / $num_executions")
    done

    arrayD_Pop_sd=()
    arrayD_World_sd=()

    #obtem o desvio padrão
    current_gen=0
    for ((i = 1; i <= num_executions; i++)); do

        path="$folderPath/execucao_$i/metrics_F$function.txt"
        while IFS= read -r line; do
            read -a densities <<<"$line"
            if [ $i -eq 1 ]; then
                arrayD_Pop_sd[$current_gen]=$(echo "scale=6; (${densities[0]} - ${arrayD_Pop[$current_gen]})^2" | bc)
                arrayD_World_sd[$current_gen]=$(echo "scale=6; (${densities[1]} - ${arrayD_World[$current_gen]})^2" | bc)
                #echo "$current_gen  ${arrayD_Pop_sd[$current_gen]}  ${arrayD_World_sd[$current_gen]}"
            else
                if [ -n "${arrayD_World_sd[$current_gen]}" ]; then
                    arrayD_Pop_sd[$current_gen]=$(echo "scale=6; ${arrayD_Pop_sd[$current_gen]} + (${densities[0]} - ${arrayD_Pop[$current_gen]})^2" | bc)
                    arrayD_World_sd[$current_gen]=$(echo "scale=6; ${arrayD_World_sd[$current_gen]} + (${densities[1]} - ${arrayD_World[$current_gen]})^2" | bc)
                    #echo "$current_gen  ${arrayD_Pop_sd[$current_gen]}  ${arrayD_World_sd[$current_gen]}"
                fi
            fi
            ((current_gen++))
        done <"$path"

        if [ $current_gen -lt $gen ]; then
            gen=$current_gen
        fi
        current_gen=0
    done
    if [ ! -d "$folderPath/metrics_all_executions" ]; then
        mkdir $folderPath/metrics_all_executions
    fi

    path_output="$folderPath/metrics_all_executions/average_sd_F$function.csv"
    if [ -f "$path_output" ]; then
        rm $path_output
    fi

    >>$path_output
    echo "P_Average,P_SD,W_Average,W_SD" >>$path_output
    for ((i = 0; i < $gen; i++)); do
        arrayD_Pop_sd[$i]=$(echo "scale=6; sqrt(${arrayD_Pop_sd[$i]} / $num_executions)" | bc)
        arrayD_World_sd[$i]=$(echo "scale=6; sqrt(${arrayD_World_sd[$i]} / $num_executions)" | bc)

        echo "${arrayD_Pop[$i]},${arrayD_Pop_sd[$i]},${arrayD_World[$i]},${arrayD_World_sd[$i]}" >>$path_output
    done

}

main


