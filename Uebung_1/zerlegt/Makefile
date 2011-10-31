CC=gcc
CCOPTS=-g -Wall -O
OBJ = include/punkt.o include/strich.o include/rest_op.o

all: gesamt

clean:
	rm gesamt

gesamt: hpt.c
	$(CC) $(CCOPTS) $(OBJ) hpt.c -o gesamt

