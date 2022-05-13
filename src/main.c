/* Standard Libraries */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

/* Allegro Libraries */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_font.h>          /* Allegro Font library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */
#include <allegro5/allegro_primitives.h>    /* Allegro Primatives library */

/* local Libraries */
#include "collisions.h"
#include "mob.h"
#include "global.h"
#include "terrain.h"
#include "random.h"
#include "mob_handler.h"

#define KEY_SEEN     1
#define KEY_RELEASED 2

#define FPS          60.0

bool show_dev_tools = false;
int dev_tool_pos    = 16;

void camera_update(float* cameraPosition, float x, float y, float width, float height, float x_max, float y_max) {
    cameraPosition[0] = -(SCREEN_WIDTH / 2) + (x + width/2);
    cameraPosition[1] = -(SCREEN_HEIGHT / 2) + (y + height/2);
    
    cameraPosition[0] = constrain_f(0, abs(x_max - SCREEN_WIDTH), cameraPosition[0]);
    cameraPosition[1] = constrain_f(0, abs(y_max - SCREEN_HEIGHT), cameraPosition[1]);
}

int main(int argc, char** argv) {
//    al_set_config_value(al_get_system_config(), "trace", "level", "debug");

    /* Initialize everything */
    assert(al_init());                  /* Allegro environment */
    assert(al_install_keyboard());      /* Install Keyboard */
    assert(al_install_mouse());         /* Install Mouse */
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

    /* Mouse Stuff */
    int mouseX = 0;
    int mouseY = 0;
    ALLEGRO_MOUSE_CURSOR* cursor  = al_create_mouse_cursor(al_load_bitmap("../assets/crosshair.png"), 0, 0);
    al_set_mouse_cursor(disp, cursor);


    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    /* Initialize random number generator */
    rng_initialize();
    
    /* Floor & room setup */
    Room* current_room;
    Floor f;
    int start_row = MAX_ROWS/2;
    int start_col = MAX_COLS/2;
    generate_floor(&f, start_row, start_col);

    current_room = &f.map[start_row][start_col];
    int status = load_room(current_room);

    /* Player Setup */
    Mob p;
    int start_player_pos_x = current_room->width/2 - PLAYER_WIDTH/2;
    int start_player_pos_y = current_room->height/2 - PLAYER_HEIGHT/2;
    p = initialize_mob(PLAYER, 0, start_player_pos_x, start_player_pos_y);

    if(status != OK) {
        printf("an error has occured. Exiting...");
        return ERROR;
    }

    /* Testing some Handler Action! */
    MOB_HANDLER mob_manager = default_mob_handler();
    initialize_handler(&mob_manager, 20);

    spawn_mobs(&mob_manager, current_room->width, current_room->height, 10);
    printf("Mob Handler mob count: %d\n", mob_manager.mob_count);
    /* Camera Setup */
    float cameraPosition[2] = {0, 0};
    ALLEGRO_TRANSFORM camera;

    /* Game Loop */
    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    /* Variables to handle timer events */
    double delta_time   = 0;
    double fps          = 0;
    double new_time     = 0;
    double old_time     = al_get_time();

    al_start_timer(timer);
    while(1) {
        al_wait_for_event(queue, &event);

        switch(event.type) {
            case ALLEGRO_EVENT_TIMER:
                /* Update fps */
                new_time = al_get_time();
                delta_time = new_time - old_time;
                fps = 1.0 / delta_time;
                old_time = new_time;

                /* Update Player info */
                p.update(key, &p, current_room->width, current_room->height);
                update_all_active_mobs(&mob_manager, current_room->width, current_room->height);
                //slime.update(NULL, &slime, current_room-> width, current_room->height);

                /* Update camera position and transform everything on the screen */
                camera_update(cameraPosition, p.position[0], p.position[1], p.width, p.height, current_room->width, current_room->height);
                al_identity_transform(&camera);
                al_translate_transform(&camera, -cameraPosition[0], -cameraPosition[1]);
                al_use_transform(&camera);

                /* Check for door collisions to change rooms */
                if(is_collision(&p.hb, &current_room->north_door) ||
                   is_collision(&p.hb, &current_room->south_door) ||
                   is_collision(&p.hb, &current_room->east_door) ||
                   is_collision(&p.hb, &current_room->west_door)) {
                  current_room = change_rooms(f.map, current_room, &p);
                }
                /* ESC key to exit game */
                if(key[ALLEGRO_KEY_ESCAPE]) {
                    done = true;
                    break;
                }
                /* T key to show dev tools */
                if(key[ALLEGRO_KEY_T]) {
                  show_dev_tools = true;
                }
                
                for(int i = 0; i < ALLEGRO_KEY_MAX; i++) {
                    key[i] &= KEY_SEEN;
                }

                redraw = true;
                break;
            case ALLEGRO_EVENT_MOUSE_AXES:
                mouseX = event.mouse.x;
                mouseY = event.mouse.y;
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
            al_clear_to_color(al_map_rgb(0, 0, 0));
            show_room(current_room);
            p.draw(&p, delta_time);
            draw_all_active_mobs(&mob_manager, delta_time);
            if(show_dev_tools) {
              al_draw_textf(font, al_map_rgb(0, 0, 0), 0, dev_tool_pos * 0, 0, "Player position. x: %d, y: %d", p.position[0], p.position[1]);
              al_draw_textf(font, al_map_rgb(0, 0, 0), 0, dev_tool_pos * 1, 0, "Current Room: %s", current_room->id);
              al_draw_textf(font, al_map_rgb(0, 0, 0), 0, dev_tool_pos * 2, 0, "FPS: %f", fps);
              al_draw_textf(font, al_map_rgb(0, 0, 0), 0, dev_tool_pos * 3, 0, "Mouse Position: %d, %d", mouseX, mouseY);
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
