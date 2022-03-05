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
  /* Defining left, right, top, and bottom lines for both hitboxes */
  int left_1   = hb1->px;
  int right_1  = hb1->px + hb1->width;
  int top_1    = hb1->py;
  int bottom_1 = hb1->py + hb1->height;
  int left_2   = hb2->px;
  int right_2  = hb2->px + hb2->width;
  int top_2    = hb2->py;
  int bottom_2 = hb2->py + hb2->height;

  /* Checks ranges for Hitbox 1 */
  bool L1_in_range = (left_1 >= left_2 && left_1 <= right_2);
  bool T1_in_range = (top_1 >= top_2 && top_1 <= bottom_2);
  bool R1_in_range = (right_1 <= right_2 && right_1 >= left_2);
  bool B1_in_range = (bottom_1 <= bottom_2 && bottom_1 >= top_2);

  /* Checks ranges for Hitbox 2 */
  bool L2_in_range = (left_2 >= left_1 && left_2 <= right_1);
  bool T2_in_range = (top_2 >= top_1 && top_2 <= bottom_1);
  bool R2_in_range = (right_2 <= right_1 && right_2 >= left_1);
  bool B2_in_range = (bottom_2 <= bottom_1 && bottom_2 >= top_1);

  /* Output */
  bool output = (L1_in_range && T1_in_range) || (L2_in_range && T2_in_range) || (R1_in_range && B1_in_range) || (R2_in_range && B2_in_range);
  if(output) printf("Collision Detected!\n");
  return output;
}
