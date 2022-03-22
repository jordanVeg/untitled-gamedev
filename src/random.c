#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "random.h"

void rng_initialize() {
    srand(time(NULL));
}

bool rng_percent_chance(double percent) {
    return (double)rand()/(double)((unsigned)RAND_MAX) <= percent;
}

int rng_random_int(int min, int max) {
    return (rand()%(max - min + 1)) + min;
}
