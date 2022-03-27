#ifndef INCLUDE_GLOBAL_H
  #define INCLUDE_GLOBAL_H

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 960

#define OK     0
#define ERROR -1

int constrain(int min, int max, int val);
float constrain_f(float min, float max, float val);

#endif