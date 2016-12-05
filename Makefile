CC=g++
CFLAGS=-g -Wall -std=c++11

TARGET=main
SRC=$(TARGET).cc
LIB=ray.o object.o

all: main

main: $(SRC) $(LIB)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIB)

ray.o: ray.cc
	$(CC) $(CFLAGS) -c ray.cc

object.o: object.cc
	$(CC) $(CFLAGS) -c object.cc

clean:
	rm $(TARGET) $(LIB)