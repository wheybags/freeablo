add_library(Cel 
    cel/celfile.cpp cel/celfile.h  
    cel/celframe.h 
    cel/pal.cpp cel/pal.h 
    cel/tiledecoding.cpp cel/tiledecoding.h 
    cel/celdecoding.cpp cel/celdecoding.h
    cel/cl2decoding.cpp cel/cl2decoding.h)

add_library(FAIO faio/faio.cpp faio/faio.h)

add_library(Levels level/dunfile.cpp  level/dunfile.h  level/minfile.cpp  level/minfile.h  level/tilfile.cpp  level/tilfile.h)

add_library(Misc misc/stringops.h)
SET_TARGET_PROPERTIES(Misc PROPERTIES LINKER_LANGUAGE CXX)
