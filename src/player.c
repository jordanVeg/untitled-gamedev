#include <stdlib.h>
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */
#include "player.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 960

int constrain(int min, int max, int val) {
    if(val <= min) {
        return min;
    } else if (val >= max ) {
        return max;
    } else {
        return val;
    }
}

int spawn_player(int start_x, int start_y, struct player* p) {
    p->pos_x = start_x;
    p->pos_y = start_y;
    p->width = PLAYER_WIDTH;
    p->height = PLAYER_HEIGHT;
    p->vel_x = 0;
    p->vel_y = 0;
    p->speed = PLAYER_SPEED;
    p->player_state = IDLE;
    p->sprite = al_load_bitmap("../assets/wizard.png");
    if(!p->sprite)
    {
        printf("Error loading player sprite!\n");
        return 1;
    }

    return 0;
}

void update_player(unsigned char key[], struct player* p) {
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
    p->pos_x = constrain(0, SCREEN_WIDTH - PLAYER_WIDTH, (p->pos_x + p->vel_x));
    p->pos_y = constrain(0, SCREEN_HEIGHT - PLAYER_HEIGHT,(p->pos_y + p->vel_y));

    /* Reset velocity to 0, so that the player doesnt move forever */
    p->vel_x = 0;
    p->vel_y = 0;
}

void show_player(struct player* p) {
    /* Draw player */
    al_draw_bitmap(p->sprite, p->pos_x, p->pos_y, 0 );
}
