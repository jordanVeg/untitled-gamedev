#ifndef INCLUDE_COLLISIONS_H
  #define INCLUDE_COLLISIONS_H

typedef struct hitbox {
  int px, py, width, height;
} Hitbox;

static const Hitbox DEFAULT_HITBOX = {-1, -1, 0, 0};

void create_hitbox(Hitbox* hb, int x, int y, int width, int height);

void update_hitbox_position(Hitbox* hb, int new_pos_x, int new_pos_y);

bool is_collision(Hitbox* hb1, Hitbox* hb2);

#endif
