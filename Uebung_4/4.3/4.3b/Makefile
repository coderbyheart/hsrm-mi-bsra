CC=gcc
CCFLAGS=-Wall -g -ansi 

SRC=b.c
PROGS=$(SRC:.c=)

all: progs

clean:
	/bin/rm -f *.o $(PROGS)

.SUFFIXES: .o .c .h

%.o : %.c
	$(CC) -c $(CCFLAGS) $< -o $@

%.o : %.c %.h
	$(CC) -c $(CCFLAGS) $< -o $@

