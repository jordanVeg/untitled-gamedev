#ifndef INCLUDE_ATTACK_H
#define INCLUDE_ATTACK_H

#include "collisions.h"

typedef struct projectile {
    int x, y, r, id, damage;
    double xspeed, yspeed;
    bool live;
    Hitbox hb;
} Projectile;

Projectile initialize_projectile(int r, int damage);

void fire_projectile(Projectile* bullet, int startx, int starty, int endx, int endy, int speed);

void update_projectile(Projectile* bullet);

void draw_projectile(Projectile* bullet);


#endif