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
# 68 "/usr/include/assert.h" 3 4



extern void __assert_fail (__const char *__assertion, __const char *__file,
      unsigned int __line, __const char *__function)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));


extern void __assert_perror_fail (int __errnum, __const char *__file,
      unsigned int __line,
      __const char *__function)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));




extern void __assert (const char *__assertion, const char *__file, int __line)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));



# 2 "assert-demo.c" 2

extern void assertOk();
extern void assertNotOk();

const int true = 1;
const int false = 0;

void assertOk()
{
 ((1 == 1) ? (void) (0) : __assert_fail ("1 == 1", "assert-demo.c", 11, __PRETTY_FUNCTION__));
 ((true) ? (void) (0) : __assert_fail ("true", "assert-demo.c", 12, __PRETTY_FUNCTION__));
}

void assertNotOk()
{
 ((1 == 2) ? (void) (0) : __assert_fail ("1 == 2", "assert-demo.c", 17, __PRETTY_FUNCTION__));
 ((false) ? (void) (0) : __assert_fail ("false", "assert-demo.c", 18, __PRETTY_FUNCTION__));
}

int main(void) {
 assertOk();
 assertNotOk();
 return 0;
}
