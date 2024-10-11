#!/bin/bash

# Verifica se o diretório alvo foi passado como argumento
if [ -z "$1" ]; then
    echo "Uso: $0 <diretório>"
    exit 1
fi

# Diretório alvo
TARGET_DIR="$1"

# Verifica se o diretório existe
if [ ! -d "$TARGET_DIR" ]; then
    echo "Erro: Diretório '$TARGET_DIR' não existe."
    exit 1
fi

# Função para remover linhas duplicadas
remove_duplicates() {
    local file="$1"
    # Remove linhas duplicadas e sobrescreve o arquivo original
    sort -u "$file" -o "$file"
    echo "Linhas duplicadas removidas de: $file"
}

# Percorre todos os arquivos no diretório e subdiretórios
find "$TARGET_DIR" -type f | while read -r file; do
    # Remove linhas duplicadas para cada arquivo encontrado
    remove_duplicates "$file"
done

echo "Processo concluído."
