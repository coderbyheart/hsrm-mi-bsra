DEPCC=$(CC) -MM
CFLAGS=-g

FILES=libmytime.c
OBJS=$(FILES:.c=.o)
DEPS=depend
LIB=libmytime.a

# Biliothek erzeugen: "$@" erzeugt das
# "Target" der Make-Regel (hier: "$(LIB)")
# und damit letztendlich (s.o) "libmemprobe.a". 
$(LIB): $(OBJS)
	$(AR) -rv $@ $(OBJS)

-include $(DEPS)

$(OBJS):

$(DEPS):
	$(DEPCC) $(FILES) >$(DEPS)

clean:
	rm -f $(OBJS) $(DEPS)

clobber: clean
	rm -f $(LIB) *~

