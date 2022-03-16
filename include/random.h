#ifndef INCLUDE_RANDOM_H
#define INCLUDE_RANDOM_H

void rng_initialize();

bool rng_in_range(double percent);

int rng_random_int(int max);

#endif