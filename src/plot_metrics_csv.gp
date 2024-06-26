#! /usr/bin/gnuplot
# Recebe o número de colunas como parâmetro



# Defina o separador de campo como vírgula
set datafile separator ","

# Configurações gerais
set style line 1 lc rgb 'black' lw 4 pt 7 ps 1.5
set style line 2 lc rgb 'red' lw 4 pt 7 ps 1.5
set grid

# Configurações de saída
#print output_path
set terminal png size 1280,720 font "Arial,20"
set output outputname

# Configurações do gráfico
unset title
set xlabel 'Generation'
set ylabel yyLabel

# Plotar todas as colunas do arquivo
#plot for [col=2:ncolumns] 'dados.csv' using 0:col with lines title sprintf("Coluna %d", col-1)
plot for [i=1:columns] filename using 0:i with lines linewidth 5 title word(nomes_colunas, i)


