CFLAGS=-g -ansi -Wall 
# CFLAGS=-ansi -O

all: assert-demo assert-demo-nodebug setjmp-demo.c

assert-demo:

assert-demo-nodebug:
	$(CC) $(CFLAGS) -DNDEBUG assert-demo.c -o $@

setjmp-demo:

memprobe: memprobe.c
	$(CC) $(CFLAGS) memprobe.c -o $@ -L ./lib -lmemprobe

memprobe1: memprobe1.c
	$(CC) $(CFLAGS) memprobe1.c -o $@ -L ./lib -lmemprobe

clean:
	rm -f assert-demo
	rm -f assert-demo-nodebug
	rm -f setjmp-demo
	rm -f memprobe
	rm -f memprobe1
