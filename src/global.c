#include <stdlib.h>

#include "global.h"

int constrain(int min, int max, int val) {
    if(val <= min) {
        return min;
    } else if (val >= max ) {
        return max;
    } else {
        return val;
    }
}