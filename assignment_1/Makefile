CC = g++
CPPFLAGS = -g -Wall -O2 -std=c++17
LDFLAGS =

SOURCEDIR = src
BUILDDIR = out

CPX_BASE    = /opt/ibm/ILOG/CPLEX_Studio128
CPX_INCDIR  = $(CPX_BASE)/cplex/include
CPX_LIBDIR  = $(CPX_BASE)/cplex/lib/x86-64_linux/static_pic
CPX_LDFLAGS = -lcplex -lm -pthread -ldl -lstdc++fs

OBJ = main.o parser.o model.o

%.o: $(SOURCEDIR)/%.cpp
		$(CC) $(CPPFLAGS) -I$(CPX_INCDIR) -c $^ -o $@

main: $(OBJ)
		$(CC) $(CPPFLAGS) $(OBJ) -o main -L$(CPX_LIBDIR) $(CPX_LDFLAGS)

clean:
		rm -rf $(OBJ) main

.PHONY: clean
