#include <stdbool.h>
#include <stdlib.h>

/* Allegro Libraries */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_primitives.h>    /* Allegro Primatives library */

#include "collisions.h"

Hitbox default_hitbox() {
  Hitbox hb = {
    .x     = -1,
    .y     = -1,
    .width  = -1,
    .height = -1
  };
  return hb;
}

void create_hitbox(Hitbox* hb, int x, int y, int width, int height) {
  hb->x     = x;
  hb->y     = y;
  hb->width  = width;
  hb->height = height;
}

void update_hitbox_position(Hitbox* hb, int new_pos_x, int new_pos_y) {
  hb->x = new_pos_x;
  hb->y = new_pos_y;
}

bool is_collision(Hitbox* hb1, Hitbox* hb2) {
  /* Defining left, right, top, and bottom lines for both hitboxes */
  int left_1   = hb1->x;
  int right_1  = hb1->x + hb1->width;
  int top_1    = hb1->y;
  int bottom_1 = hb1->y + hb1->height;
  int left_2   = hb2->x;
  int right_2  = hb2->x + hb2->width;
  int top_2    = hb2->y;
  int bottom_2 = hb2->y + hb2->height;

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
  //if(output) printf("Collision Detected!\n");
  return output;
}

void draw_hitbox(Hitbox* hb, ALLEGRO_COLOR color) {
  al_draw_rectangle(hb->x, hb->y, hb->x + hb->width, hb->y + hb->height, color, 1);
}
