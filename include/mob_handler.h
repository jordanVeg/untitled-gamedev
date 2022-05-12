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
//#define DEFAULT_MOB_HANDLER {(DEFAULT_MOB), 0, 0}

typedef struct mob_handler {
    Mob mobs[ABSOLUTE_MAX_MOBS];
    int local_max_mobs;
    int mob_count;
} MOB_HANDLER;

MOB_HANDLER default_mob_handler();

void initialize_handler(MOB_HANDLER* handler, int max_mobs);

int add_mob(MOB_HANDLER* handler, Mob mob);

int remove_mob(MOB_HANDLER* handler, Mob mob);

#endif