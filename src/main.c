/* Standard Libraries */
#include <stdio.h>
#include <string.h>
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
#include "attack.h"

#define KEY_SEEN     1
#define KEY_RELEASED 2

#define FPS          60.0

bool show_dev_tools = false;
int dev_tool_pos    = 16;

typedef enum game_state {
    GS_RUNNING,
    GS_MENU
} Game_State;

void camera_update(float* cameraPosition, float x, float y, float width, float height, float x_max, float y_max) {
    cameraPosition[0] = -(SCREEN_WIDTH / 2) + (x + width/2);
    cameraPosition[1] = -(SCREEN_HEIGHT / 2) + (y + height/2);

    cameraPosition[0] = constrain_f(0, abs(x_max - SCREEN_WIDTH), cameraPosition[0]);
    cameraPosition[1] = constrain_f(0, abs(y_max - SCREEN_HEIGHT), cameraPosition[1]);
}

void initialize_game_state(Mob* p_p, Floor* f_p) {
    Mob p;
    Room r;

    generate_floor(f_p, 1, MAX_ROWS/2, MAX_COLS/2);

    r = f_p->map[MAX_ROWS/2][MAX_COLS/2];

    int start_player_pos_x = r.width/2 - PLAYER_WIDTH/2;
    int start_player_pos_y = r.height/2 - PLAYER_HEIGHT/2;
    p = initialize_mob(PLAYER, 0, start_player_pos_x, start_player_pos_y);
    memcpy(p_p, &p, sizeof(Mob));
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

    /* Initialize global game state variables */
    Mob p = default_mob();
    Floor f;
    Room* current_room = 0;
    Game_State current_game_state = GS_MENU;

    /* Projectile Testing */
    Projectile bullet1 = initialize_projectile(5, 10);

    Projectile bullet2 = initialize_projectile(10, 20);
    //Projectile live_bullets[10] = {initialize_projectile(0,0)};


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

        if(current_game_state == GS_RUNNING) {
            switch(event.type) {
                case ALLEGRO_EVENT_TIMER:
                    /* Update fps */
                    new_time = al_get_time();
                    delta_time = new_time - old_time;
                    fps = 1.0 / delta_time;
                    old_time = new_time;

                    /* Update Player */
                    p.update(key, &p, current_room->width, current_room->height);

                    /* Hitbox collisions -- Needs to be updated, very temporary */
                    for(int i = 0; i < current_room->m_handler_p->local_max_mobs; i++) {
                        if(is_collision(&p.hb, &current_room->m_handler_p->mobs[i].hb)){
                            p.current_health -= 10;
                        }
                    }

                    if(p.current_state == DEAD) {
                        // STRETCH: End Run screen with stats.
                        // clear all keyboard inputs, change game state to menu
                        // TODO: Free memory of everything --> dungeon, mobs, etc.
                        current_game_state = GS_MENU;
                        memset(key, 0, sizeof(key));
                        printf("dead.\n");
                        break;
                    }

                    /* Update Projectile */
                    update_projectile(&bullet1);
                    update_projectile(&bullet2);

                    /* Check for bullet collisions */
                    for(int i = 0; i < current_room->m_handler_p->local_max_mobs; i++) {
                        if(is_collision(&bullet1.hb, &current_room->m_handler_p->mobs[i].hb)) {
                            current_room->m_handler_p->mobs[i].current_health -= bullet1.damage;
                            bullet1.live = false;
                            break;
                        }
                        if(is_collision(&bullet2.hb, &current_room->m_handler_p->mobs[i].hb)) {
                            current_room->m_handler_p->mobs[i].current_health -= bullet2.damage;
                            bullet2.live = false;
                            break;
                        }
                    }

                    /* Update camera position and transform everything on the screen */
                    camera_update(cameraPosition, p.position[0], p.position[1], p.width, p.height, current_room->width, current_room->height);
                    al_identity_transform(&camera);
                    al_translate_transform(&camera, -cameraPosition[0], -cameraPosition[1]);
                    al_use_transform(&camera);

                    /* Update all elements of the dungeon */
                    current_room = update_dungeon_state(&f, current_room, &p);

                    /* ESC key to exit game */
                    if(key[ALLEGRO_KEY_ESCAPE]) {
                        done = true;
                        break;
                    }

                    /* T key to show dev tools */
                    if(key[ALLEGRO_KEY_T]) {
                        show_dev_tools = true;
                    }

                    /* Update all keys in array to keep movement smooth */
                    for(int i = 0; i < ALLEGRO_KEY_MAX; i++) {
                        key[i] &= KEY_SEEN;
                    }

                    redraw = true;
                    break;
                case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                    /* Fire Bullet */
                    if(event.mouse.button == 1) {
                        fire_projectile(&bullet1, p.position[0] + p.width/2, p.position[1] + p.height/2, mouseX, mouseY, 50);
                    }
                    if(event.mouse.button == 2) {
                        fire_projectile(&bullet2, p.position[0] + p.width/2, p.position[1] + p.height/2, mouseX, mouseY, 10);
                    }
                    break;
                case ALLEGRO_EVENT_MOUSE_AXES:
                    /* Update Mouse Position */
                    mouseX = event.mouse.x;
                    mouseY = event.mouse.y;
                    break;
                case ALLEGRO_EVENT_KEY_DOWN:
                    /* Update key array with button press */
                    key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;

                    /* K kill all mobs in the room */
                    if(key[ALLEGRO_KEY_K]) {
                        reset_handler(current_room->m_handler_p);
                        p.current_health -= 10;
                    }
                    /* Turn on Hitboxes*/
                    if(key[ALLEGRO_KEY_H]) {
                        toggle_hitboxes();
                    }
                    /* Temp -- Move to next floor */
                    if(key[ALLEGRO_KEY_E]) {
                        if(current_room->type == R_EXIT && f.key_found) {
                            Floor new_floor;
                            Room new_room;
                            generate_floor(&new_floor, f.number+1, current_room->row_pos, current_room->col_pos);
                            /* Insert Loading Screen or spawning animation here */
                            new_room = new_floor.map[current_room->row_pos][current_room->col_pos];
                            load_room(&new_room);
                            unload_room(current_room);
                            current_room = &new_room;
                            destroy_floor(&f);
                            f = new_floor;
                        }
                        else if(current_room->type == R_KEY && !f.key_found) {
                            f.key_found = true;
                        }
                    }
                    break;
                case ALLEGRO_EVENT_KEY_UP:
                    /* Update key array when key is released */
                    key[event.keyboard.keycode] &= KEY_RELEASED;
                    break;
                case ALLEGRO_EVENT_DISPLAY_CLOSE:
                    /* Use the 'X' button to close the game */
                    done = true;
                    break;
            }
        } else {
            /* 'MENU' game state processing */
            switch(event.type) {
                case ALLEGRO_EVENT_TIMER:
                     /* ESC key to exit game */
                    if(key[ALLEGRO_KEY_ESCAPE]) {
                        done = true;
                        break;
                    }
                    /* ENTER key*/
                    if(key[ALLEGRO_KEY_ENTER]) {
                        current_game_state = GS_RUNNING;
                        /* clears keyboard inputs */
                        memset(key, 0, sizeof(key));
                        /* Initialize Dungeon and Load Room */
                        initialize_game_state(&p, &f);
                        current_room = &f.map[MAX_ROWS/2][MAX_COLS/2];
                        load_room(current_room);
                        break;
                    }
                    for(int i = 0; i < ALLEGRO_KEY_MAX; i++) {
                        key[i] &= KEY_SEEN;
                    }
                    redraw = true;
                    break;
                case ALLEGRO_EVENT_DISPLAY_CLOSE:
                    done = true;
                    break;
                case ALLEGRO_EVENT_KEY_DOWN:
                    key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                    break;
                default:
                    break;
            }
        }

        if(done) break;
        if(redraw && al_is_event_queue_empty(queue)) {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            if(current_game_state == GS_RUNNING) {
                draw_room(current_room, f.texture_p, delta_time);
                p.draw(&p, delta_time);
                draw_projectile(&bullet1);
                draw_projectile(&bullet2);
                al_draw_textf(font, al_map_rgb(0, 0, 0), 0, dev_tool_pos * 0, 0, "key found: %d", f.key_found);
                if(show_dev_tools) {
                al_draw_textf(font, al_map_rgb(0, 0, 0), 0, dev_tool_pos * 1, 0, "Player position. x: %d, y: %d", p.position[0], p.position[1]);
                al_draw_textf(font, al_map_rgb(0, 0, 0), 0, dev_tool_pos * 2, 0, "Current Room: %d - %s", f.number, current_room->id);
                al_draw_textf(font, al_map_rgb(0, 0, 0), 0, dev_tool_pos * 3, 0, "FPS: %f", fps);
                al_draw_textf(font, al_map_rgb(0, 0, 0), 0, dev_tool_pos * 4, 0, "Mouse Position: %d, %d", mouseX, mouseY);
                }
            }
            else if(current_game_state == GS_MENU) {
                al_draw_textf(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2, 0, "Press [ENTER] to Begin");
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
