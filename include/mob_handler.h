#ifndef INCLUDE_MOB_HANDLER_H
#define INCLUDE_MOB_HANDLER_H

/* Allegro Libraries */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_font.h>          /* Allegro Font library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */
#include <allegro5/allegro_primitives.h>    /* Allegro Primatives library */

#include "mob.h"
#include "global.h"

#define ABSOLUTE_MAX_MOBS 100

typedef struct mob_handler {
    Mob mobs[ABSOLUTE_MAX_MOBS];
    int local_max_mobs;
    int mob_count;
    bool is_initialized;
} Mob_Handler;

Mob_Handler default_mob_handler();

void initialize_handler(Mob_Handler* handler, int max_mobs);

void reset_handler(Mob_Handler* handler);

int add_mob(Mob_Handler* handler, Mob mob);

int remove_mob(Mob_Handler* handler, Mob* mob);

void update_all_active_mobs(Mob_Handler* handler, int max_px, int max_py);

void draw_all_active_mobs(Mob_Handler* handler, double delta_time);

void spawn_mobs(Mob_Handler* handler, int max_px, int max_py, int floor_number);

#endif
