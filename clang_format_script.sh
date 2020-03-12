#!/bin/bash -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"

FMT_COMMAND=clang-format-7

$FMT_COMMAND -version

paths="apps components test"

for x in $paths; do 
    find $x -name *.h -o -name *.cpp | xargs $FMT_COMMAND -i -style=file
done

if [ ! -z "$TRAVIS" ]; then
    DIFF_FILE=/tmp/fa_format_diff.patch

    git diff > $DIFF_FILE

    if [ -s $DIFF_FILE ]; then
        DIFF_URL=$(cat "$DIFF_FILE" | nc termbin.com 9999 | tr -d '\0')
        echo -e "\e[31m" # red
        echo "Autoformat needed, to apply the formatting changes locally, run:"
        echo "curl $DIFF_URL > $DIFF_FILE && git apply $DIFF_FILE"
        echo -e "\e[0m" # end red
        exit 1
    fi
fi

exit 0
