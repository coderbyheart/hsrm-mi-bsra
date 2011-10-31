# 1 "assert-demo.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "assert-demo.c"
# 1 "/usr/include/assert.h" 1 3 4
# 37 "/usr/include/assert.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 313 "/usr/include/features.h" 3 4
# 1 "/usr/include/bits/predefs.h" 1 3 4
# 314 "/usr/include/features.h" 2 3 4
# 346 "/usr/include/features.h" 3 4
# 1 "/usr/include/sys/cdefs.h" 1 3 4
# 353 "/usr/include/sys/cdefs.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 354 "/usr/include/sys/cdefs.h" 2 3 4
# 347 "/usr/include/features.h" 2 3 4
# 378 "/usr/include/features.h" 3 4
# 1 "/usr/include/gnu/stubs.h" 1 3 4



# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 5 "/usr/include/gnu/stubs.h" 2 3 4


# 1 "/usr/include/gnu/stubs-32.h" 1 3 4
# 8 "/usr/include/gnu/stubs.h" 2 3 4
# 379 "/usr/include/features.h" 2 3 4
# 38 "/usr/include/assert.h" 2 3 4
# 2 "assert-demo.c" 2

extern void assertOk();
extern void assertNotOk();

const int true = 1;
const int false = 0;

void assertOk()
{
 ((void) (0));
 ((void) (0));
}

void assertNotOk()
{
 ((void) (0));
 ((void) (0));
}

int main(void) {
 assertOk();
 assertNotOk();
 return 0;
}
