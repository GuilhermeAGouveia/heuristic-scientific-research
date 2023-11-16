echo "Coletando as informações de entrada para o furaaf.sh"
echo "Executando o make"

export COD_NAME="getInputForFuraaf"
make

echo "Limpando ilhas.txt"
rm -rf ilhas.txt

for i in $(cat irace/results/results-island-model/results/results-evol-1-25.txt | grep "./" | cut -d'l' -f2 | sed "s/ /_/g"); do 
    params=$(echo $i | sed "s/_/ /g" | sed "s/A/N/g"); 
    ./evol $params >> ilhas.txt;  
done