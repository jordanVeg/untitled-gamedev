#include <stdlib.h>

#include "global.h"

bool show_hitboxes = false;

int constrain(int min, int max, int val) {
    if(val <= min) {
        return min;
    } else if (val >= max ) {
        return max;
    } else {
        return val;
    }
}

float constrain_f(float min, float max, float val) {
    if(val <= min) {
        return min;
    } else if (val >= max ) {
        return max;
    } else {
        return val;
    }
}

void toggle_hitboxes() {
    show_hitboxes = !show_hitboxes;
}