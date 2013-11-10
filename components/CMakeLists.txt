add_library(Cel cel/celfile.cpp cel/celfile.h  cel/celframe.h cel/pal.cpp cel/pal.h)

add_library(FAIO faio/faio.cpp faio/faio.h)

add_library(Levels levels/dunfile.cpp  levels/dunfile.h  levels/minfile.cpp  levels/minfile.h  levels/tilfile.cpp  levels/tilfile.h)

add_library(Misc misc/stringops.h)
SET_TARGET_PROPERTIES(Misc PROPERTIES LINKER_LANGUAGE CXX)
