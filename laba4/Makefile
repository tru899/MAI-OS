CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LIB_CFLAGS = -fPIC -shared 

SRC_DIR = src
INC_DIR = include

all: lib1.so lib2.so prog1 prog2

lib1.so: $(SRC_DIR)/lib1.c $(INC_DIR)/libs.h
	$(CC) $(CFLAGS) $(LIB_CFLAGS) -o lib1.so $(SRC_DIR)/lib1.c -lm

lib2.so: $(SRC_DIR)/lib2.c $(INC_DIR)/libs.h
	$(CC) $(CFLAGS) $(LIB_CFLAGS) -o lib2.so $(SRC_DIR)/lib2.c -lm

prog1: $(SRC_DIR)/prog1.c $(INC_DIR)/libs.h lib1.so
	$(CC) $(CFLAGS) -o prog1 $(SRC_DIR)/prog1.c ./lib1.so -lm

prog2: $(SRC_DIR)/prog2.c
	$(CC) $(CFLAGS) -o prog2 $(SRC_DIR)/prog2.c -ldl

clean:
	rm -f *.so prog1 prog2
