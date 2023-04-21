#!/bin/sh

if [[ $(ls | wc -l) > 5 ]]; then
    echo "W bieżącym katalogu znajduje się więcej niż 5 plików"
fi
