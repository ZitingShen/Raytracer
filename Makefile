CC=g++
CFLAGS=-O2 -g -Wall -std=c++11

TARGET=main
SRC=$(TARGET).cc
LIB=ray.o object.o transformation.o read_ppm.o

all: main

main: $(SRC) $(LIB)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIB)

ray.o: ray.cc
	$(CC) $(CFLAGS) -c ray.cc

object.o: object.cc
	$(CC) $(CFLAGS) -c object.cc

transformation.o: transformation.cc
	$(CC) $(CFLAGS) -c transformation.cc

read_ppm.o: read_ppm.c
	$(CC) $(CFLAGS) -c read_ppm.c

clean:
	rm $(TARGET) $(LIB)
