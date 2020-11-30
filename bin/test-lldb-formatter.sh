#!/usr/bin/env bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 path/to/test-executable"
    exit 1
fi

exec=$1

lldb --no-lldbinit -b -s tests/prettydebug.lldb -- $1 2>&1 >/dev/null | diff - tests/prettydebug-expected.txt
