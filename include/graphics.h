#ifndef INCLUDE_GRAPHICS_H
#define INCLUDE_GRAPHICS_H

#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */

typedef struct animation {
    ALLEGRO_BITMAP sprite_sheet;
    int number_of_frames;
    int height;
    int width;
    int target_fps;
    int max_loop_count;  //number of times it should loop before stopping. -1 for indefinite.
} Animation;

void play_animation(Animation a);




#endif