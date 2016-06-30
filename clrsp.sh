#!/usr/bin/env bash
# Remove whitespaces on EOL in source files.
# Fridolin Pokorny
# fridex.devel@gmail.com
# 2012

function remove_white_space {
    (rm -f "$1" ; sed 's/[[:space:]]*$//' > "$1" ) < "$1"
}

export -f remove_white_space

find . -name "*.cpp" \
    -exec bash -c 'remove_white_space "{}"' \;

find . -name "*.h" \
    -exec bash -c 'remove_white_space "{}"' \;

