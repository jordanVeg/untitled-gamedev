#ifndef INCLUDE_PLAYER_H
  #define INCLUDE_PLAYER_H

#include "collisions.h"

#define PLAYER_WIDTH  64
#define PLAYER_HEIGHT 64
#define PLAYER_SPEED  8

/* Should help support animations later on */
typedef enum {
    IDLE,
    RUNNING,
    DASH,
    HIT,
    DEAD
} STATE;

typedef struct player {
    int pos_x;
    int pos_y;
    int width;
    int height;
    int vel_x;
    int vel_y;
    int speed;
    int dir;
    Hitbox hb;
    STATE player_state;
    int last_animation_frame;
    float animation_tracker;
    ALLEGRO_BITMAP* sprite;
} Player;

int initialize_player(Player* p);

int spawn_player(int start_x, int start_y, Player* p);

void update_player(unsigned char key[], Player* p, int max_px, int max_py);

void show_player(Player* p, double delta_time);

#endif