CC = g++
CPPFLAGS = -g -Wall -O2
LDFLAGS =

SOURCEDIR = src
BUILDDIR = out

OBJ = main.o

%.o: $(SOURCEDIR)/%.cpp
		$(CC) $(CPPFLAGS) -c $^ -o $@

main: $(OBJ)
		$(CC) $(CPPFLAGS) $(OBJ) -o main

clean:
		rm -rf $(OBJ) main

.PHONY: clean
