
# nonsugar
#
# Copyright iorate 2016.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

CXX := g++
CXXFLAGS := -Wall -Wextra --pedantic-errors -I../include
LDFLAGS := -lboost_unit_test_framework-mt
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
PROGRAM := nonsugar_test.exe

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf $(PROGRAM) $(OBJS)
