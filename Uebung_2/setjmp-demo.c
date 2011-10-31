#include <stdio.h>
#include <setjmp.h>

static jmp_buf env;

void rec(int i)
{
	if (i > 5) longjmp(env, 1);
	putchar('.');
	rec(++i);
}

int main(void) {
	if(!setjmp(env)) {
		rec(1);
	} else {
		printf("Done.\n");
	}
	return 0;
}
