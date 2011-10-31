#include <assert.h>

extern void assertOk();
extern void assertNotOk();

const int true = 1;
const int false = 0;

void assertOk()
{
	assert(1 == 1);
	assert(true);
}

void assertNotOk()
{
	assert(1 == 2);
	assert(false);
}

int main(void) {
	assertOk();
	assertNotOk();
	return 0;
}
