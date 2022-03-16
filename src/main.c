/* Standard Libraries */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* Allegro Libraries */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_font.h>          /* Allegro Font library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */
#include <allegro5/allegro_primitives.h>    /* Allegro Primatives library */

/* local Libraries */
#include "collisions.h"
#include "player.h"
#include "global.h"
#include "terrain.h"
#include "random.h"

#define KEY_SEEN     1
#define KEY_RELEASED 2

#define FPS          60.0

bool show_dev_tools = false;

int main(int argc, char** argv) {
//    al_set_config_value(al_get_system_config(), "trace", "level", "debug");

    /* Initialize everything */
    assert(al_init());                  /* Allegro environment */
    assert(al_install_keyboard());      /* Install Keyboard */
    assert(al_init_primitives_addon()); /* Initialize Primatives */
    assert(al_init_image_addon());      /* Initialize Image Library */

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    if(!timer) {
        printf("couldn't initialize timer\n");
        return ERROR;
    }

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    if(!queue) {
        printf("couldn't initialize queue\n");
        return ERROR;
    }

    ALLEGRO_DISPLAY* disp = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if(!disp) {
        printf("couldn't initialize display\n");
        return ERROR;
    }

    ALLEGRO_FONT* font = al_create_builtin_font();
    if(!font) {
        printf("couldn't initialize font\n");
        return ERROR;
    }

    /* Set up keyboard for fluid keyboard events */
    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    /* Initialize random number generator */
    rng_initialize();
    
    /* Player Setup */
    Player p;
    int status = initialize_player(&p);
    status += spawn_player(SCREEN_WIDTH/2 - PLAYER_WIDTH/2, SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2, &p);

    /* Floor & room setup*/
    Room* current_room;
    Floor f;
    int start_row = MAX_ROWS/2;
    int start_col = MAX_COLS/2;
    generate_floor(&f, start_row, start_col);

    /* Testing out creating a hitbox */
    current_room = &f.map[start_row][start_col];
    status += load_room(current_room);
    if(status != OK) {
        printf("an error has occured. Exiting...");
        return ERROR;
    }
    /* Hitbox testing */
    Hitbox new_hb;
    create_hitbox(&new_hb,  SCREEN_WIDTH/2 - DOOR_HEIGHT/2, SCREEN_HEIGHT - DOOR_WIDTH, DOOR_HEIGHT, DOOR_WIDTH);

    /* Game Loop */
    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    while(1) {
        al_wait_for_event(queue, &event);

        switch(event.type) {
            case ALLEGRO_EVENT_TIMER:
                // game logic goes here.
                update_player(key, &p);
                /*
                if(is_collision(&p.hb, &new_hb)) {
                  printf("COLLISION DETECTED!\n");
                }
                */
                if(is_collision(&p.hb, &current_room->north_door) ||
                   is_collision(&p.hb, &current_room->south_door) ||
                   is_collision(&p.hb, &current_room->east_door) ||
                   is_collision(&p.hb, &current_room->west_door)) {
                  current_room = change_rooms(f.map, current_room, &p);
                }
                if(key[ALLEGRO_KEY_ESCAPE]) {
                    done = true;
                }
                if(key[ALLEGRO_KEY_T]) {
                  show_dev_tools = true;
                }
                for(int i = 0; i < ALLEGRO_KEY_MAX; i++) {
                    key[i] &= KEY_SEEN;
                }

                redraw = true;
                break;
            case ALLEGRO_EVENT_KEY_DOWN:
                 key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                 break;
            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue)) {
            show_room(current_room);
            //al_draw_rectangle(new_hb.px, new_hb.py, new_hb.px + new_hb.width, new_hb.py + new_hb.height, al_map_rgb(0, 0, 0), 10 );
            show_player(&p);
            if(show_dev_tools) {
              al_draw_textf(font, al_map_rgb(0, 0, 0), 0, 0, 0, "Player position. x: %d, y: %d", p.pos_x, p.pos_y);
            }
            al_flip_display();

            redraw = false;
        }
    }

    unload_room(current_room);
    al_destroy_bitmap(p.sprite);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return OK;
}
