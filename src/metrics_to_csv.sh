#!/bin/bash

main() {
    path="$1"
    if [ $# -eq 0 ]; then
        path="logs_genetica/metrics"
    fi

    cd "$path" || exit

    for dir in */; do
        cd "$dir" || continue

        for ex_dir in $(find . -type d -name 'execucao*'); do
            ex_dir_path=$(realpath "$ex_dir")

            for file in $(find . -type f -name 'metrics*.txt'); do
                file_path=$(realpath "$file")
                #echo "$file_path"
                file_output=$(echo "$file" | sed "s/.txt/.csv/g")
                echo "P,W" >"$file_output"
                sed "s/ /,/g" "$file" >>"$file_output"
                rm $file
            done
        done

        cd .. || exit
    done
}

main "$1"
