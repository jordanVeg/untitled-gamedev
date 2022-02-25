#include <stdbool.h>
#include <stdlib.h>

#include "collisions.h"

void create_hitbox(Hitbox* hb, int x, int y, int width, int height) {
  hb->px     = x;
  hb->py     = y;
  hb->width  = width;
  hb->height = height;
}

void update_hitbox_position(Hitbox* hb, int new_pos_x, int new_pos_y) {
  hb->px = new_pos_x;
  hb->py = new_pos_y;
}

bool is_collision(Hitbox* hb1, Hitbox* hb2) {
  /*
  * This needs to be tweaked. It doesnt detect collisions if the top left
  * of either structures arent touching. Not quite sure how to fix it yet,
  * but it definitely needs to be done. 
  */
  int left_1_x   = hb1->px;
  int right_1_x  = hb1->px + hb1->width;
  int top_1_y    = hb1->py;
  int bottom_1_y = hb1->py + hb1->height;
  int left_2_x   = hb2->px;
  int right_2_x  = hb2->px + hb2->width;
  int top_2_y    = hb2->py;
  int bottom_2_y = hb2->py + hb2->height;

  bool x1_in_range = (left_1_x >= left_2_x && left_1_x <= right_2_x);
  bool y1_in_range = (top_1_y >= top_2_y && top_1_y <= bottom_2_y); 
  bool x2_in_range = (left_2_x >= left_1_x && left_2_x <= right_1_x);
  bool y2_in_range = (top_2_y >= top_1_y && top_2_y <= bottom_1_y);

  return (x1_in_range && y1_in_range) || (x2_in_range && y2_in_range); 
}
