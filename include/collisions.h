#ifndef INCLUDE_COLLISIONS_H
#define INCLUDE_COLLISIONS_H

#include <allegro5/allegro5.h>

typedef struct hitbox {
  int x, y, width, height;
} Hitbox;

Hitbox default_hitbox();

void create_hitbox(Hitbox* hb, int x, int y, int width, int height);

void update_hitbox_position(Hitbox* hb, int new_pos_x, int new_pos_y);

bool is_collision(Hitbox* hb1, Hitbox* hb2);

void draw_hitbox(Hitbox* hb, ALLEGRO_COLOR color);

#endif
