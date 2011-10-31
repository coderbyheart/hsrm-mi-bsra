CC=gcc
CCFLAGS= -Wall -g -ansi
LD=gcc
DEPCC=$(CC) -MM
DEPS=depend

SRC=fisch.c
EXTRA=fisch-args.c fisch-help.c fischdb.c
OBJS=$(EXTRA:.c=.o)
PROGS=$(SRC:.c=)

all: $(PROGS)

$(DEPS):
	$(DEPCC) $(SRC) > $(DEPS)
	
$(PROGS): $(OBJS)

clean:
	-/bin/rm -f *.o $(PROGS) fisch.db

.SUFFIXES: .o .c .h

%.o : %.c
	$(CC) -c $(CCFLAGS) $< -o $@

%.o : %.c %.h
	$(CC) -c $(CCFLAGS) $< -o $@
	
-include $(DEPS)

test: clean $(PROGS)
	./fisch -n schmidt glupschi
	./fisch -n schmidt gunther
	./fisch -n schmidt fritz
	./fisch -n huber achim
	./fisch -n huber guido
	./fisch -n huber julien
	./fisch -n mueller herbert
	./fisch -n mueller frank
	./fisch -n mueller herrmann
	./fisch -l
	./fisch -l meier
	./fisch -l mueller
	./fisch -z schmidt 22
	./fisch -l
	./fisch -d huber
	./fisch -d meier
	./fisch -l

test-error: clean $(PROGS)
	-./fisch -n schmidt glupschi extra > /dev/null
	-./fisch -l meier extra > /dev/null
	-./fisch -z schmidt 22 extra > /dev/null
	-./fisch -z schmidt extra > /dev/null
	-./fisch -z schmidt extra 22 > /dev/null
	-./fisch -z schmidt > /dev/null
	-./fisch -d huber extra > /dev/null