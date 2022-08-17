CC = g++
SRC = src/minesweeper2.cpp
HEADER = include/minesweeper2.h 
CFLAGS = -c -Wall -Iinclude

minesweeper2: main.o minesweeper2.o
	$(CC) -o $@ main.o minesweeper2.o

main.o: src/main.cpp $(HEADER)
	$(CC) $(CFLAGS) $< -o $@

minesweeper2.o: $(SRC) $(HEADER)
	$(CC) $(CFLAGS) $< -o $@