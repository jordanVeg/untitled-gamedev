#ifndef INCLUDE_MOB_H
#define INCLUDE_MOB_H

#include "collisions.h"

#define PLAYER_WIDTH  64
#define PLAYER_HEIGHT 64
#define PLAYER_SPEED  8

/* Should help support animations later on */
typedef enum animation_state {
    IDLE,
    RUNNING,
    DASH,
    HIT,
    DEAD
} STATE;

typedef enum mob_type {
  DEFAULT,
  PLAYER,
  SLIME
} MOB_TYPE;

typedef struct mob {
    int position[2];
    int id;
    int width;
    int height;
    int vel_x;
    int vel_y;
    int speed;
    int dir;
    MOB_TYPE type;
    Hitbox hb;
    STATE current_state;
    int last_animation_frame;
    float animation_tracker;
    ALLEGRO_BITMAP* sprite;

    void (*update)(unsigned char key[], struct mob* self, int max_px, int max_py);
    void (*draw)(struct mob* self, double delta_time);
} Mob;

Mob default_mob();

Mob initialize_mob(MOB_TYPE type, int id, int start_x, int start_y);

void move_mob(Mob* mob, int new_xpos, int new_ypos);

#endif