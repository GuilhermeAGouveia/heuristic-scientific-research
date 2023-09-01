seq="1 2 3 4 5 6 7 8 9 10"
for i in $seq; do
    rm -f target-runner
    cat target-runner-model | sed "s/<ISLAND_NUMBER>/$i/g" > target-runner
    chmod +x target-runner
    irace > result-irace-$i.txt
done;

wait
shutdown -h now