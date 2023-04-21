#!/bin/bash

if [[ "$(ls | wc -l)" > 5 ]]; then
    echo "W katalogu jest więcej niż 5 plików"
fi
