#ifndef INCLUDE_RANDOM_H
#define INCLUDE_RANDOM_H

void rng_initialize();

bool rng_percent_chance(double percent);

int rng_random_int(int min, int max);

#endif