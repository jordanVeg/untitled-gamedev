/* Standard Includes */
#include <stdlib.h>
#include <stdio.h>

/* Allegro Includes */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */

/* Local Includes */
#include "mob.h"
#include "random.h"
#include "global.h"

#define PLAYER_ANIMATION_FPS 8

int spawn_mob(int spawn_x, int spawn_y, Mob* mob) {
    mob->position[0] = spawn_x;
    mob->position[1] = spawn_y;

    update_hitbox_position(&mob->hb,  mob->position[0],  mob->position[1]);
    return OK;
}

void update_player(unsigned char key[], Mob* p, int max_px, int max_py) {
    /* Update speed based on Button press */
    p->current_state = IDLE;
    if(key[ALLEGRO_KEY_W]) {
        p->vel_y = -p->speed;
        p->current_state = RUNNING;
    }
    if(key[ALLEGRO_KEY_S]) {
        p->vel_y = p->speed;
        p->current_state = RUNNING;
    }
    if(key[ALLEGRO_KEY_A]) {
        p->vel_x = -p->speed;
        p->current_state = RUNNING;
        p->dir = 0;
    }
    if(key[ALLEGRO_KEY_D]) {
        p->vel_x = p->speed;
        p->current_state = RUNNING;
        p->dir = 1;
    }
    /* Update position based on speed */
    p->position[0] = constrain(0, max_px - p->width, (p->position[0] + p->vel_x));
    p->position[1] = constrain(0, max_py - p->height,(p->position[1] + p->vel_y));
    update_hitbox_position(&p->hb,  p->position[0],  p->position[1]);

    /* Reset velocity to 0, so that the player doesnt move forever */
    p->vel_x = 0;
    p->vel_y = 0;
    
}

void draw_player(Mob* p, double delta_time) {
    /* Draw player */
    int sourceX = 0;
    int sourceY = 0;
    float animation_update_time = (1.0 / PLAYER_ANIMATION_FPS);
    p->animation_tracker += delta_time;
    switch(p->current_state) {
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
    al_draw_bitmap_region(p->sprite, sourceX, sourceY, p->width, p->height, p->position[0], p->position[1], flip_flag );
    p->last_animation_frame = sourceX;
}

int initialize_player(Mob* p) {
  p->position[0] = 0;
  p->position[1] = 0;
  p->width = PLAYER_WIDTH;
  p->height = PLAYER_HEIGHT;
  p->vel_x = 0;
  p->vel_y = 0;
  p->speed = PLAYER_SPEED;
  p->current_state = IDLE;
  p->dir = 0;
  p->type = PLAYER;
  create_hitbox(&p->hb, 0, 0, PLAYER_WIDTH, PLAYER_HEIGHT);
  p->last_animation_frame = 0;
  p->animation_tracker = 0.0;
  p->sprite = al_load_bitmap("../assets/wizard.png");
  if(!p->sprite) {
      printf("Error loading player sprite!\n");
      return ERROR;
  }
  p->update = update_player;
  p->draw = draw_player;
  return OK;
}

void update_slime(unsigned char key[], Mob* slime, int max_px, int max_py) {
    slime->vel_x = (slime->dir == 0)? slime->speed : -slime->speed;
    
    if(slime->position[0] <= 0) slime->dir = 0;
    if((slime->position[0] + slime->width) >= max_px) slime->dir = 1;
    
    slime->position[0] = constrain(0, max_px - slime->width, (slime->position[0] + slime->vel_x));
}

void draw_slime(Mob* slime, double delta_time) {
    al_draw_bitmap(slime->sprite, slime->position[0], slime->position[1], 0);
}

int initialize_slime(Mob* m) {
    m->position[0] = 0;
    m->position[1] = 0;
    m->width = 32;
    m->height = 32;
    m->vel_x = 0;
    m->vel_y = 0;
    m->speed = 4;
    m->current_state = IDLE;
    m->dir = 0;
    m->type = SLIME;
    create_hitbox(&m->hb, 0, 0, 32, 32);
    m->last_animation_frame = 0;
    m->animation_tracker = 0.0;
    m->sprite = al_load_bitmap("../assets/slime.png");
    if(!m->sprite) {
        printf("Error loading slime sprite!\n");
        return ERROR;
    }
    m->update = update_slime;
    m->draw = draw_slime;
    return OK;  
}


