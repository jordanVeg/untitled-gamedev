#include <stdbool.h>
#include <stdlib.h>


bool is_collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
  int px1 = x1;
  int pxw1 = x1 + w1;
  int py1 = y1;
  int pyh1 = y1 + h1;

  int px2 = x2;
  int pxw2 = x2 + w2;
  int py1 = y2;
  int pyh1 = y2 + h2;

  return (px1 <= pxw2 && px1 >= px2) && (py1 <= pyh2 && py1 >= py2 );
}
