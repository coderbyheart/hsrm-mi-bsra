CC=gcc
CCFLAGS= -Wall -g -DDEBUG
LD=gcc
DEPCC=$(CC) -MM
DEPS=depend

SRC=fisch.c
EXTRA=dijkstra.c rwlock.c
OBJS=$(EXTRA:.c=.o)
PROGS=$(SRC:.c=)

all: $(PROGS)

$(DEPS):
	$(DEPCC) $(SRC) > $(DEPS)

$(PROGS): $(OBJS)

doc:
	doxygen doxyscript

clean:
	-/bin/rm -f *.o $(PROGS)

.SUFFIXES: .o .c .h

%.o : %.c
	$(CC) -c $(CCFLAGS) $< -o $@

%.o : %.c %.h
	$(CC) -c $(CCFLAGS) $< -o $@

-include $(DEPS)