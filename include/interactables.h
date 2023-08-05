#ifndef INCLUDE_INTERACTABLES_H
#define INCLUDE_INTERACTABLES_H

#include <stdbool.h>
#include "mob.h"

typedef struct interactable {
    int length;
    int width;
    int pos_x;
    int pos_y;
    int range_r;
    bool active;
} Interactable;

void set_interactable_active(bool active);

bool is_player_within_range(Interactable* obj, Mob* mob);

void draw_interactable(Interactable* obj);

#endif