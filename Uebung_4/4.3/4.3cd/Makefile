CFLAGS=-g -ansi -Wall 
# CFLAGS=-ansi -O
DEPS=depend
LIBDIR=lib

all: 
	cd lib; make
	make mytime

mytime: mytime.c
	$(CC) $(CFLAGS) -o $@ mytime.c -L${LIBDIR} -lmytime

clean:
	rm -f mytime
