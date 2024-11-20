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

contar_instancias() {
    pgrep -f "furaaf.sh" | grep -v "^$$\$" | wc -l
}

# last_three_digits=$(echo $$ | sed 's/.*\(...\)$/\1/')
# sleep_time=$(echo "scale=1; ($last_three_digits / 1.8)" | bc)

# # Coloca o script para dormir para que as diferentes instâncias não acessem params.txt ao mesmo tempo
# sleep $sleep_time
# echo "Processo $$ dormiu por $sleep_time segundos."

# Caminho do arquivo de lock
lock_file="paramsLock"


# Tenta obter o lock para acessar params.txt
while ! mkdir "$lock_file" 2>/dev/null; do
    echo "Processo $$ aguardando pelo lock."
    sleep 0.5
done

echo "Processo $$ obteve o lock."

echo "Data e hora atual: $(date +"%Y-%m-%d %H:%M:%S.%3N")"

file_input=params.txt

# Obtém a primeira linha do arquivo params.txt 
config=$(head -n 1 "$file_input" | sed "s/ /_/g")

sed -i '1d' "$file_input"

rm -rf $lock_file
echo $config


# Executa o script principal
./furaaf_main.sh "$config"
