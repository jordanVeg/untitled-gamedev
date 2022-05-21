#ifndef INCLUDE_GLOBAL_H
  #define INCLUDE_GLOBAL_H

#include <stdbool.h>

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 960

#define OK     0
#define ERROR -1

extern bool show_hitboxes;

int constrain(int min, int max, int val);
float constrain_f(float min, float max, float val);

void toggle_hitboxes();

#endif