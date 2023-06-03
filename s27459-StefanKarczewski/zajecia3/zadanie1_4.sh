#!/bin/bash

if [[ "$(find -type f -maxdepth 1 | wc -l)" -gt 5 ]]; then
    echo "W katalogu jest więcej niż 5 plików"
fi
