#!/bin/bash -e

if [ "$TRAVIS_OS_NAME" == "osx" ] && [ $JOB != "build" ]; then
    exit 0;
fi

if [ $JOB == "clang-format" ]; then
    ./clang_format_script.sh
fi

if [ $JOB == "build" ]; then
    mkdir build
    cd build
    cmake -DTREAT_WARNINGS_AS_ERRORS=ON -G "Unix Makefiles" ../
    make -k
    make fatest -k
fi
