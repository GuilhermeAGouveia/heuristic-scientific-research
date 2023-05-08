#!/bin/bash

for i in $(seq 12 15); do 
    ./coleta-info.sh -n 100 -t 10 -c evolucao_simples -f $i >> result_evolucao_simples.txt
done;