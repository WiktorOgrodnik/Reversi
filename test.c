#include <stdio.h>
#include <stdint.h>

int main() {

	printf("%X\n", ((__uint128_t)1 << 127) >> 127);
}