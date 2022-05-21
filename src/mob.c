/* Standard Includes */
#include <stdlib.h>
#include <stdio.h>

/* Allegro Includes */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */
#include <allegro5/allegro_primitives.h>    /* Allegro Primatives library */

/* Local Includes */
#include "mob.h"
#include "random.h"
#include "global.h"

#define PLAYER_ANIMATION_FPS 24

Mob default_mob() {
    Mob mob = {
        .position               = {-1},
        .id                     = -1,
        .width                  = -1,
        .height                 = -1,
        .vel_x                  = -1,
        .vel_y                  = -1,
        .speed                  = -1,
        .dir                    = -1,
        .max_health             = 0.0,
        .current_health         = 0.0,
        .type                   = DEFAULT,
        .hb                     = default_hitbox(),
        .current_state          = IDLE,
        .last_animation_frame   = -1,
        .animation_tracker      = -1,
        .sprite                 = NULL
    };
    return mob;
}

void update_player(unsigned char key[], Mob* p, int max_px, int max_py) {
    if(p->current_health <= 0) {
        p->current_state = DEAD;
    }
    else {
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
}

void update_slime(unsigned char key[], Mob* slime, int max_px, int max_py) {
    if (slime->current_health <= 0) {
        slime->current_state = DEAD;
    }
    else {
        slime->vel_x = (slime->dir == 0)? slime->speed : -slime->speed;
        
        if(slime->position[0] <= 0) slime->dir = 0;
        if((slime->position[0] + slime->width) >= max_px) slime->dir = 1;
        
        slime->position[0] = constrain(0, max_px - slime->width, (slime->position[0] + slime->vel_x));
    }
    update_hitbox_position(&slime->hb, slime->position[0], slime->position[1]);
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
    al_draw_bitmap_region(m->sprite, sourceX, sourceY, m->width, m->height, m->position[0], m->position[1], flip_flag);
    /* 
    *  Im gonna try to implement a health bar because im too lazy to import a
    *  font. This actually works pretty well. 
    *  TODO: make this a "health bar" function or something, maybe I can
    *  generalize it to represent other values as well? 
    */
    if(m->current_health != m->max_health) {
        al_draw_rectangle(m->position[0], m->position[1] - 10, m->position[0] + m->width, m->position[1] - 5, al_map_rgb(0, 100, 0), 5);
        al_draw_rectangle(m->position[0], m->position[1] - 10, m->position[0] + (m->width - (m->width * constrain_f(0, 1, m->current_health/m->max_health))), m->position[1] - 5, al_map_rgb(100, 0, 0), 5);
    }
    m->last_animation_frame = sourceX;
    if(show_hitboxes) {
        draw_hitbox(&m->hb, al_map_rgb(255, 0, 0));
    }
}

void draw_static_mob(Mob* m, double delta_time) {
    al_draw_bitmap(m->sprite, m->position[0], m->position[1], 0);
}

Mob initialize_mob(MOB_TYPE type, int id, int start_x, int start_y) {
    Mob m;
    m.position[0]          = start_x;
    m.position[1]          = start_y;
    m.id                   = id;
    m.vel_x                = 0;
    m.vel_y                = 0;
    m.current_state        = IDLE;
    m.dir                  = 0;
    m.type                 = type;
    m.last_animation_frame = 0;
    m.animation_tracker    = 0.0;

    switch(type){
        case PLAYER:
            m.width  = PLAYER_WIDTH;
            m.height = PLAYER_HEIGHT;
            m.speed  = PLAYER_SPEED;
            m.max_health = 100;
            m.sprite = al_load_bitmap("../assets/wizard.png");
            m.update = update_player;
            m.draw   = draw_mob;
            break;
        case SLIME:
            m.width  = 32;
            m.height = 32;
            m.speed  = rng_random_int(6, 10);
            m.max_health = 30;
            m.sprite = al_load_bitmap("../assets/slime.png");
            m.update = update_slime;
            m.draw   = draw_mob;   
            break;
        default:
            m.width  = 0;
            m.height = 0;
            m.speed  = 0;
            m.max_health = 0;
            m.sprite = NULL;
            m.update = update_slime;
            m.draw   = draw_mob;
            break;
    }
    m.current_health = m.max_health;
    
    if(!m.sprite && type != DEFAULT) {
        printf("Error loading sprite!\n");
    }
    create_hitbox(&m.hb, m.position[0], m.position[1], m.width, m.height);
    return m;
}

void move_mob(Mob* mob, int new_xpos, int new_ypos) {
    mob->position[0] = new_xpos;
    mob->position[1] = new_ypos;
    update_hitbox_position(&mob->hb,  mob->position[0],  mob->position[1]);
}


