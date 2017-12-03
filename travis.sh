#!/bin/bash -e

if [ $JOB == "clang-format" ]; then
    ./clang_format_script.sh
fi

if [ $JOB == "build" ]; then
    mkdir build
    cd build
    cmake -DTREAT_WARNINGS_AS_ERRORS=ON ../
    make -k
    make fatest -k
fi
