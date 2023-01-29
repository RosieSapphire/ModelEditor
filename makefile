CC=gcc
INC=-Iinclude
LIB=$(shell pkg-config --libs glfw3) $(shell pkg-config --libs glew) -lm

CFLAGS=-std=c99 -Wall -Wextra -g3 -Iinclude

SRC=main.c mat4.c shader.c vec4.c vec3.c
OBJ=main.o mat4.o shader.o vec4.o vec3.o

BIN=model_editor

all: $(OBJ)
	clear
	$(CC) $(CFLAGS) $(INC) -o $(BIN) $(addprefix src/,$(SRC)) $(LIB)
	rm -rf *.o

%.o: src/%.c
	$(CC) $(CFLAGS) $(INC) -c $^ $(LIB)

clean:
	rm -rf $(BIN) *.o
	clear
