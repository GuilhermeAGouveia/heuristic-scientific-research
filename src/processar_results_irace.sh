#!/bin/bash

# Verifique se o número correto de argumentos foi fornecido
if [ $# -ne 2 ]; then
    echo "Uso: $0 arquivo_de_entrada.txt arquivo_de_saida.txt"
    exit 1
fi

# Nome do arquivo de entrada
arquivo_entrada="$1"

# Nome do arquivo de saída
arquivo_saida="$2"

# Verifique se o arquivo de entrada existe
if [ ! -f "$arquivo_entrada" ]; then
    echo "O arquivo de entrada '$arquivo_entrada' não existe."
    exit 1
fi

# Limpe o arquivo de saída (se existir)
> "$arquivo_saida"

# Loop através das linhas do arquivo de entrada
while IFS= read -r linha; do
    # Verifique se a linha não está em branco
    if [ -n "$linha" ]; then
        # Adicione uma mensagem de depuração
        echo "Executando: ./evol $linha"

        # Execute ./evol antes de cada linha e redirecione a saída para o arquivo de saída
        ./evol $linha >> "$arquivo_saida"

        # Adicione outra mensagem de depuração
        echo "Resultado redirecionado para $arquivo_saida"
    fi
done < "$arquivo_entrada"

echo "Processamento concluído. Resultados salvos em '$arquivo_saida'."
