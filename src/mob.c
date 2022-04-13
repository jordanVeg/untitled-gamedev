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

void draw_mob(Mob* m, double delta_time) {
    /* Draw player */
    int sourceX = 0;
    int sourceY = 0;
    float animation_update_time = (1.0 / m->speed);
    m->animation_tracker += delta_time;
    switch(m->current_state) {
        case IDLE:
            sourceY = 0;
            break;
        case RUNNING:
            sourceY = m->height;
            break;
        default:
            sourceX = 0;
            sourceY = 0;
            break;
    }
    if(m->animation_tracker >= animation_update_time) {
        sourceX = (m->last_animation_frame + m->width);
        sourceX = (sourceX >= m->width*4) ? 0 : sourceX;
        m->animation_tracker = 0.0;
    } else {
        sourceX = m->last_animation_frame;
    }
    int flip_flag = m->dir == 0 ? 0 : ALLEGRO_FLIP_HORIZONTAL;
    al_draw_bitmap_region(m->sprite, sourceX, sourceY, m->width, m->height, m->position[0], m->position[1], flip_flag );
    m->last_animation_frame = sourceX;
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

int initialize_mob(Mob* m, MOB_TYPE type) {
    m->position[0] = 0;
    m->position[1] = 0;
    m->vel_x = 0;
    m->vel_y = 0;
    m->current_state = IDLE;
    m->dir = 0;
    m->type = type;
    m->last_animation_frame = 0;
    m->animation_tracker = 0.0;

    switch(type){
        case PLAYER:
            m->width = PLAYER_WIDTH;
            m->height = PLAYER_HEIGHT;
            m->speed = PLAYER_SPEED;
            m->sprite = al_load_bitmap("../assets/wizard.png");
            m->update = update_player;
            m->draw = draw_mob;
            break;
        case SLIME:
            m->width = 32;
            m->height = 32;
            m->speed = 6;
            m->sprite = al_load_bitmap("../assets/slime.png");
            m->update = update_slime;
            m->draw = draw_mob;   
            break;
    }
    
    if(!m->sprite) {
        printf("Error loading sprite!\n");
        return ERROR;
    }
    create_hitbox(&m->hb, 0, 0, m->width, m->height);
    return OK;
}


