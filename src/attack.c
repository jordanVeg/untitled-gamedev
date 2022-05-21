/* Standard Includes */
#include <stdio.h>
#include <math.h>

/* Allegro Includes */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */
#include <allegro5/allegro_primitives.h>    /* Allegro Primatives library */

#include "attack.h"


PROJECTILE initialize_projectile() {
    PROJECTILE bullet = {
        .x = 0,
        .y = 0,
        .r = 0,
        .xspeed = 0,
        .yspeed = 0,
        .live = false,
        .hb = default_hitbox()
    };
    return bullet;
}

/*
*  Create projectile at a specified point with a target point in mind. 
*/
void fire_projectile(PROJECTILE* bullet, int startx, int starty, int endx, int endy, int speed) {
    double theta = atan2(endy - starty, endx - startx);

    bullet->x = startx;
    bullet->y = starty;
    bullet->r = 5;
    create_hitbox(&bullet->hb, bullet->x, bullet->y, bullet->r, bullet->r);
    bullet->xspeed = speed * cos(theta);
    bullet->yspeed = speed * sin(theta);
    bullet->live = true; 
}

/*
*  Update projectile position.
*/
void update_projectile(PROJECTILE* bullet) {
    if(bullet->live) {
        bullet->x += bullet->xspeed;
        bullet->y += bullet->yspeed;
        update_hitbox_position(&bullet->hb, bullet->x, bullet->y);
    }
}

/*
*  Draw projectile on the screen. Temporarily draw a simple circle.
*/
void draw_projectile(PROJECTILE* bullet) {
    if(bullet->live) {
        al_draw_filled_circle(bullet->x, bullet->y, bullet->r, al_map_rgb(255, 255, 255));
    }
}