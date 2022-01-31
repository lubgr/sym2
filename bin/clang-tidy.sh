#!/usr/bin/env bash

if [[ $# -ne 1 || ! -f $1 ]]; then
    echo "Usage: $0 [compile command database (json)]"
    exit 1
fi

files=`git ls-files -- '*.h' '*.cpp'`

for file in `ls $files`; do
    clang-tidy -quiet -header-filter='tsym' -p "$1" "${file}"
done
