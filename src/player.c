/* Standard Includes */
#include <stdlib.h>
#include <stdio.h>

/* Allegro Includes */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */

/* Local Includes */
#include "player.h"
#include "global.h"


int initialize_player(Player* p) {
  p->pos_x = 0;
  p->pos_y = 0;
  p->width = PLAYER_WIDTH;
  p->height = PLAYER_HEIGHT;
  p->vel_x = 0;
  p->vel_y = 0;
  p->speed = PLAYER_SPEED;
  p->player_state = IDLE;
  create_hitbox(&p->hb, 0, 0, PLAYER_WIDTH, PLAYER_HEIGHT);
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
    if(key[ALLEGRO_KEY_W]) {
        p->vel_y = -p->speed;
    }
    if(key[ALLEGRO_KEY_S]) {
        p->vel_y = p->speed;
    }
    if(key[ALLEGRO_KEY_A]) {
        p->vel_x = -p->speed;
    }
    if(key[ALLEGRO_KEY_D]) {
        p->vel_x = p->speed;
    }
    /* Update position based on speed */
    p->pos_x = constrain(0, max_px - PLAYER_WIDTH, (p->pos_x + p->vel_x));
    p->pos_y = constrain(0, max_py - PLAYER_HEIGHT,(p->pos_y + p->vel_y));
    update_hitbox_position(&p->hb,  p->pos_x,  p->pos_y);

    /* Reset velocity to 0, so that the player doesnt move forever */
    p->vel_x = 0;
    p->vel_y = 0;
}

void show_player(struct player* p) {
    /* Draw player */
    al_draw_bitmap(p->sprite, p->pos_x, p->pos_y, 0 );
}
