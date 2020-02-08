# Copyright 2014 by Martin Moene
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# optional lite is inspired on std::optional by Fernando Cacciola and
# Andrzej Krzemienski and on expected lite by Martin Moene.

# Usage: gmake [STD=c++03]

PROGRAM = optional-main.t
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

ifdef STD
STD_OPTION = -std=$(STD)
endif

CXX = g++
CXXFLAGS = $(STD_OPTION) -I../include -Wall  # -Wextra

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) -o $@ $^

test: $(PROGRAM)
	./$(PROGRAM)

test-all: $(PROGRAM)
	./$(PROGRAM) @

list: test
	./$(PROGRAM) -l

clean:
	$(RM) $(OBJECTS)
	$(RM) $(PROGRAM)

