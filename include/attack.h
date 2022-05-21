#ifndef INCLUDE_ATTACK_H
#define INCLUDE_ATTACK_H

#include "collisions.h"

typedef struct projectile {
    int x, y, r, id, damage;
    double xspeed, yspeed;
    bool live;
    Hitbox hb;
} PROJECTILE;

PROJECTILE initialize_projectile();

void fire_projectile(PROJECTILE* bullet, int startx, int starty, int endx, int endy, int speed);

void update_projectile(PROJECTILE* bullet);

void draw_projectile(PROJECTILE* bullet);


#endif