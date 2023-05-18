#!/bin/bash

for i in $(seq 1 15); do 
    ./coleta-info.sh -n 100 -c evolucao_simples -f $i >> result_evolucao_simples.txt
done;