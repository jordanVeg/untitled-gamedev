/* Standard Includes */
#include <stdlib.h>
#include <stdio.h>

/* Allegro Includes */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */

/* Local Includes */
#include "player.h"
#include "global.h"

#define PLAYER_ANIMATION_FPS 8


int initialize_player(Player* p) {
  p->pos_x = 0;
  p->pos_y = 0;
  p->width = PLAYER_WIDTH;
  p->height = PLAYER_HEIGHT;
  p->vel_x = 0;
  p->vel_y = 0;
  p->speed = PLAYER_SPEED;
  p->player_state = IDLE;
  p->dir = 0;
  create_hitbox(&p->hb, 0, 0, PLAYER_WIDTH, PLAYER_HEIGHT);
  p->last_animation_frame = 0;
  p->animation_tracker = 0.0;
  p->sprite = al_load_bitmap("../assets/wizard.png");
  if(!p->sprite) {
      printf("Error loading player sprite!\n");
      return ERROR;
  }

  return OK;
}

int spawn_player(int start_x, int start_y, Player* p) {
    p->pos_x = start_x;
    p->pos_y = start_y;

    update_hitbox_position(&p->hb,  p->pos_x,  p->pos_y);
    return OK;
}

void update_player(unsigned char key[], Player* p, int max_px, int max_py) {
    /* Update speed based on Button press */
    p->player_state = IDLE;
    if(key[ALLEGRO_KEY_W]) {
        p->vel_y = -p->speed;
        p->player_state = RUNNING;
    }
    if(key[ALLEGRO_KEY_S]) {
        p->vel_y = p->speed;
        p->player_state = RUNNING;
    }
    if(key[ALLEGRO_KEY_A]) {
        p->vel_x = -p->speed;
        p->player_state = RUNNING;
        p->dir = 0;
    }
    if(key[ALLEGRO_KEY_D]) {
        p->vel_x = p->speed;
        p->player_state = RUNNING;
        p->dir = 1;
    }
    /* Update position based on speed */
    p->pos_x = constrain(0, max_px - PLAYER_WIDTH, (p->pos_x + p->vel_x));
    p->pos_y = constrain(0, max_py - PLAYER_HEIGHT,(p->pos_y + p->vel_y));
    update_hitbox_position(&p->hb,  p->pos_x,  p->pos_y);

    /* Reset velocity to 0, so that the player doesnt move forever */
    p->vel_x = 0;
    p->vel_y = 0;
    
}

void show_player(struct player* p, double delta_time) {
    /* Draw player */
    int sourceX = 0;
    int sourceY = 0;
    float animation_update_time = (1.0 / PLAYER_ANIMATION_FPS);
    p->animation_tracker += delta_time;
    switch(p->player_state) {
        case IDLE:
            sourceY = 0;
            break;
        case RUNNING:
            sourceY = p->height;
            break;
        default:
            sourceX = 0;
            sourceY = 0;
            break;
    }
    if(p->animation_tracker >= animation_update_time) {
        sourceX = (p->last_animation_frame + p->width);
        sourceX = (sourceX >= p->width*4) ? 0 : sourceX;
        p->animation_tracker = 0.0;
    } else {
        sourceX = p->last_animation_frame;
    }
    int flip_flag = p->dir == 0 ? 0 : ALLEGRO_FLIP_HORIZONTAL;
    al_draw_bitmap_region(p->sprite, sourceX, sourceY, p->width, p->height, p->pos_x, p->pos_y, flip_flag );
    p->last_animation_frame = sourceX;
}
