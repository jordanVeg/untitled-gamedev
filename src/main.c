/* Standard Libraries */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
//#include <math.h>

/* Allegro Libraries */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_font.h>          /* Allegro Font library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */
#include <allegro5/allegro_primitives.h>    /* Allegro Primatives library */

/* local Libraries */
#include "collisions.h"
#include "player.h"

#define KEY_SEEN     1
#define KEY_RELEASED 2

#define FPS           60.0
#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 960

#define PLAYER_WIDTH  64
#define PLAYER_HEIGHT 64
#define PLAYER_SPEED  16

#define DOOR_HEIGHT (2 * PLAYER_HEIGHT)
#define DOOR_WIDTH  32

typedef enum {
  BASIC,
  KEY,
  EXIT,
  SHOP,
  CHALLENGE
} ROOM_TYPE;

/* Room and floor control */
typedef struct room {
    int width, height, num_doors, id;
    char* path_to_map_image;
    ALLEGRO_BITMAP* map;
    ALLEGRO_BITMAP* door;
    ROOM_TYPE type;
    bool is_initialized, is_loaded, is_spawnable;
    /* door array: {north, south, east, west} */

    struct room* north;
    struct room* south;
    struct room* east;
    struct room* west;
} Room;

void generate_room(Room* r, int ID, char* image_path) {
    r->width = SCREEN_WIDTH;
    r->height = SCREEN_HEIGHT;
    r->num_doors = 4;
    r->type = BASIC;
    r->is_spawnable = false;
    r->is_loaded = false;
    r->id = ID;
    r->path_to_map_image = image_path;
    r->is_initialized = true;

    r->north = NULL;
    r->south = NULL;
    r->east = NULL;
    r->west = NULL;
}

int load_room(Room* r) {
  if(r->is_initialized && !r->is_loaded) {
    r->map = al_load_bitmap(r->path_to_map_image);
    r->door = al_load_bitmap("../assets/door.png");
    if(!r->map || !r->door) {
        printf("couldn't load room map image.\n");
        return ERROR;
    }
    r->is_loaded = true;
    return OK;
  } else {
    printf("Room already loaded or not initialized.\n");
    return ERROR;
  }
}

int unload_room(Room* r) {
  if(r->is_loaded) {
    al_destroy_bitmap(r->map);
    al_destroy_bitmap(r->door);
    r->is_loaded = false;
    return OK;
  } else {
    printf("Room is not loaded, and cannot be unloaded.\n");
    return ERROR;
  }
}

Room* change_rooms(Room* current_room, Player* p) {
  /* TODO: implement exception handling via status */
  int status;
  if (p->pos_x == 0 && current_room->west) {
    status = load_room(current_room->west);
    status = unload_room(current_room);
    spawn_player(SCREEN_WIDTH-PLAYER_WIDTH-1, p->pos_y, p);
    return current_room->west;
  } else if (p->pos_x == SCREEN_WIDTH - PLAYER_WIDTH && current_room->east) {
    status = load_room(current_room->east);
    status = unload_room(current_room);
    spawn_player(1, p->pos_y, p);
    return current_room->east;
  } else if (p->pos_y == 0 && current_room->north) {
    status = load_room(current_room->north);
    status = unload_room(current_room);
    spawn_player(p->pos_x, SCREEN_HEIGHT - PLAYER_HEIGHT - 1, p);
    return current_room->north;
  } else if (p->pos_y == SCREEN_HEIGHT - PLAYER_HEIGHT && current_room->south) {
    status = load_room(current_room->south);
    status = unload_room(current_room);
    spawn_player(p->pos_x, 1, p);
    return current_room->south;
  } else {
      printf("Nowhere to go\n");
      return current_room;
  }
}

void link_rooms(Room* ref, Room* north, Room* south, Room* east, Room* west) {
  ref->north = north;
  ref->south = south;
  ref->east = east;
  ref->west = west;
}

void show_room(Room* r) {
    al_draw_bitmap(r->map, 0, 0, 0);

    /* draw doors of the room as well.. */
    if(r->west != NULL) {
      al_draw_bitmap(r->door, 0, SCREEN_HEIGHT/2 - DOOR_HEIGHT/2, 0);
    }
    if(r->east != NULL) {
      al_draw_bitmap(r->door, SCREEN_WIDTH - DOOR_WIDTH, SCREEN_HEIGHT/2 - DOOR_HEIGHT/2, ALLEGRO_FLIP_HORIZONTAL);
    }
    if(r->north != NULL) {
      al_draw_rotated_bitmap(r->door, 0, DOOR_HEIGHT/2, SCREEN_WIDTH/2, 0, ALLEGRO_PI/2, 0);
    }
    if(r->south != NULL) {
      al_draw_rotated_bitmap(r->door, 0, DOOR_HEIGHT/2, SCREEN_WIDTH/2, SCREEN_HEIGHT - DOOR_WIDTH, ALLEGRO_PI/2, ALLEGRO_FLIP_HORIZONTAL);
    }
}

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

    /* Player Setup (hopefully) */
    Player p;
    int status = initialize_player(&p);
    status += spawn_player(SCREEN_WIDTH/2 - PLAYER_WIDTH/2, SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2, &p);

    /* Room functionality testing */
    Room r1, r2, r3, r4, r5, r6, r7, r8, r9;
    Room* current_room;
    /* Generate all the rooms on the floor */
    generate_room(&r1, 1, "../assets/forest_1.png");
    generate_room(&r2, 2, "../assets/forest_2.png");
    generate_room(&r3, 3, "../assets/forest_3.png");
    generate_room(&r4, 4, "../assets/forest_4.png");
    generate_room(&r5, 5, "../assets/forest_5.png");
    generate_room(&r6, 6, "../assets/forest_6.png");
    generate_room(&r7, 7, "../assets/forest_7.png");
    generate_room(&r8, 8, "../assets/forest_8.png");
    generate_room(&r9, 9, "../assets/forest_9.png");

    /* Link all the rooms together */
    link_rooms(&r1, &r3, &r4, &r5, &r2);
    link_rooms(&r2, &r6, &r7, &r1, NULL);
    link_rooms(&r3, NULL, &r1, &r8, &r6);
    link_rooms(&r4, &r1, NULL, &r9, &r7);
    link_rooms(&r5, &r8, &r9, NULL, &r1);
    link_rooms(&r6, NULL, &r2, &r3, NULL);
    link_rooms(&r7, &r2, NULL, &r4, NULL);
    link_rooms(&r8, NULL, &r5, NULL, &r3);
    link_rooms(&r9, &r5, NULL, NULL, &r4);
    /* Testing out creating a hitbox */
    Hitbox hb;
    create_hitbox(&hb, 0, 0, PLAYER_WIDTH, PLAYER_HEIGHT);
    current_room = &r1;
    status += load_room(current_room);
    if(status != OK) {
        printf("an error has occured. Exiting...");
        return ERROR;
    }

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
                if(p.pos_x == 0 || p.pos_x == SCREEN_WIDTH - PLAYER_WIDTH || p.pos_y == 0 || p.pos_y == SCREEN_HEIGHT - PLAYER_HEIGHT) {
                  current_room = change_rooms(current_room, &p);
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
