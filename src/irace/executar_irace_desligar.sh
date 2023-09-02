seq="26 44"
for i in $seq; do
    rm -f target-runner
    cat target-runner-model | sed "s/<ISLAND_NUMBER>/$i/g" > target-runner
    chmod +x target-runner
    irace > result-irace-$i.txt
done;

wait
#shutdown -h now