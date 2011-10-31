PROGS=mycp
TEST=testdatei10MB
LIBDIR="../4.3cd/lib/"

all: $(PROGS)

$(TEST):
	dd if=/dev/urandom of=$@ bs=1M count=10
	
mycp:
	$(CC) $(CFLAGS) -o $@ $@.c -L${LIBDIR} -lmytime

test: $(PROGS) $(TEST)
	./mycp $(TEST) junk 1024

clean:
	rm -f $(PROGS) $(TEST) junk
