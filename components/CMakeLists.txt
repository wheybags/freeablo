add_library(Cel 
    cel/celfile.cpp cel/celfile.h  
    cel/celframe.h 
    cel/pal.cpp cel/pal.h 
    cel/tiledecoding.cpp cel/tiledecoding.h 
    cel/celdecoding.cpp cel/celdecoding.h
    cel/cl2decoding.cpp cel/cl2decoding.h)

add_library(FAIO faio/faio.cpp faio/faio.h)

add_library(Levels levels/dunfile.cpp  levels/dunfile.h  levels/minfile.cpp  levels/minfile.h  levels/tilfile.cpp  levels/tilfile.h)

add_library(Misc misc/stringops.h)
SET_TARGET_PROPERTIES(Misc PROPERTIES LINKER_LANGUAGE CXX)
