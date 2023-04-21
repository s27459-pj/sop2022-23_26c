#!/bin/bash

filename="$1" # file cw6data.txt


while read -r line; do
    if echo "$line" | grep -q "Mateusz"; then
        echo -e "\e[31m$line\e[0m"
    else
        echo "$line"
    fi
done < "$filename"
