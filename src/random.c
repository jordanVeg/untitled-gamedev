#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "random.h"

void rng_initialize() {
    srand(time(NULL));
}

bool rng_in_range(double percent) {
    return (double)rand()/(double)((unsigned)RAND_MAX) <= percent;
}

int rng_random_int(int max) {
    return (int)rand()%max;
}
