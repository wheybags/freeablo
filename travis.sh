#!/bin/bash -e

if [ "$TRAVIS_OS_NAME" == "osx" ]; then
    pip3 install requests
fi

if [ "$TRAVIS_OS_NAME" == "osx" ] && [ $JOB != "build" ]; then
    exit 0;
fi

if [ $JOB == "clang-format" ]; then
    ./clang_format_script.sh
fi

if [ $JOB == "build" ]; then
    mkdir build
    cd build
    cmake -DFA_TREAT_WARNINGS_AS_ERRORS=ON -G "Unix Makefiles" ../
    make -k -j2
    ./unit_tests
fi
