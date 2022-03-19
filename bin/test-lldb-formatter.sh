#!/usr/bin/env bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 path/to/pretty-printer-tests"
    exit 1
fi

exec=$1

lldb --no-lldbinit -b -s tests/prettydebug.lldb -- $1 2>&1 >/dev/null \
    | grep -v 'need to add support for DW_TAG_base_type .*auto.* encoded with DW_ATE' \
    | sed 's/ *(libsystem_m.dylib`.*//' \
    | sed 's/ *(libm.*`.*//' \
    | diff - tests/prettydebug-expected.txt
