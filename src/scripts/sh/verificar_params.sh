# O script abaixo verifica se o arquivo params.txt existe e se não existe, cria um arquivo com os parâmetros padrões.
opcao="padrao"
if [ ! -f params.txt ]; then
    echo "O arquivo params.txt não existe. Deseja criar um padrão ou personalizar? (p/padrao ou c/customizado)"
    read -p "Opção: " opcao
    case $opcao in
        p|padrao)
            echo "Criando arquivo params.txt com parâmetros padrões"
            echo '-A 1,0,0,0,0 -K 1\n-A 0,0,0,0,1 -K 1' > params.txt
            ;;
        c|customizado)
            while true; do
                echo "Digite os parâmetros separados por espaço"
                read -p "Parâmetros: " params
                echo "$params" > params.txt
                echo "Deseja adicionar mais parâmetros? (s/n)"
                read -p "Opção: " opcao
                case $opcao in
                    s|sim)
                        continue
                        ;;
                    n|nao)
                        break
                        ;;
                    *)
                        echo "Opção inválida"
                        exit 1
                        ;;
                esac
            done;
            ;;
        *)
            echo "Opção inválida"
            exit 1
            ;;
    esac
fi