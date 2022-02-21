/* Standard Libraries */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* Allegro Libraries */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_font.h>          /* Allegro Font library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */
#include <allegro5/allegro_primitives.h>    /* Allegro Primatives library */


#define OK     0
#define ERROR -1

#define KEY_SEEN     1
#define KEY_RELEASED 2

#define FPS           30.0
#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 960


#define PLAYER_WIDTH  64
#define PLAYER_HEIGHT 64
#define PLAYER_SPEED  16

#define WEST  0
#define EAST  1
#define NORTH 2
#define SOUTH 3

int constrain(int min, int max, int val) {
    if(val <= min) {
        return min;
    } else if (val >= max) {
        return max;
    } else {
        return val;
    }
}

/* Should help support animations later on */
typedef enum {
    IDLE,
    RUNNING,
    DASH,
    HIT,
    DEAD
} STATE;

/* Player code, to be moved into it's own area */
typedef struct player {
    int pos_x;
    int pos_y;
    int width;
    int height;
    int vel_x;
    int vel_y;
    int speed;
    STATE player_state;
    ALLEGRO_BITMAP* sprite;
/*
    void (*initialize)(struct player* p);
    void (*spawn)(int start_x, int start_y, struct player* p);
    void (*update)(struct player* p);
*/
} Player;
int initialize_player(struct player* p) {
  p->pos_x = 0;
  p->pos_y = 0;
  p->width = PLAYER_WIDTH;
  p->height = PLAYER_HEIGHT;
  p->vel_x = 0;
  p->vel_y = 0;
  p->speed = PLAYER_SPEED;
  p->player_state = IDLE;
  p->sprite = al_load_bitmap("../assets/wizard.png");
  if(!p->sprite)
  {
      printf("Error loading player sprite!\n");
      return ERROR;
  }

  return OK;
}

int spawn_player(int start_x, int start_y, struct player* p) {
    p->pos_x = start_x;
    p->pos_y = start_y;
    return OK;
}

void update_player(unsigned char key[], struct player* p) {
    /* Update speed based on Button press */
    if(key[ALLEGRO_KEY_W]) {
        p->vel_y = -p->speed;
    }
    if(key[ALLEGRO_KEY_S]) {
        p->vel_y = p->speed;
    }
    if(key[ALLEGRO_KEY_A]) {
        p->vel_x = -p->speed;
    }
    if(key[ALLEGRO_KEY_D]) {
        p->vel_x = p->speed;
    }
    /* Update position based on speed */
    p->pos_x = constrain(0, SCREEN_WIDTH - PLAYER_WIDTH, (p->pos_x + p->vel_x));
    p->pos_y = constrain(0, SCREEN_HEIGHT - PLAYER_HEIGHT,(p->pos_y + p->vel_y));

    /* Reset velocity to 0, so that the player doesnt move forever */
    p->vel_x = 0;
    p->vel_y = 0;
}

void show_player(struct player* p) {
    /* Draw player */
    al_draw_bitmap(p->sprite, p->pos_x, p->pos_y, 0);
}
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
    ROOM_TYPE type;
    bool is_initialized, is_loaded, is_spawnable;

    struct room* north;
    struct room* south;
    struct room* east;
    struct room* west;
} Room;

void generate_room(struct room* r, int ID, char* image_path) {
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

int load_room(struct room* r) {
  if(r->is_initialized && !r->is_loaded) {
    r->map = al_load_bitmap(r->path_to_map_image);
    if(!r->map) {
        printf("couldn't load room map image.\n");
        return ERROR;
    }
    r->is_loaded = true;
    printf("Room %i is loaded.\n",r->id);
    return OK;
  } else {
    printf("Room already loaded or not initialized.\n");
    return ERROR;
  }
}

int unload_room(struct room* r) {
  if(r->is_loaded) {
    al_destroy_bitmap(r->map);
    r->is_loaded = false;
    printf("Room %i is unloaded.\n",r->id);
    return OK;
  } else {
    printf("Room is not loaded, and cannot be unloaded.\n");
    return ERROR;
  }
}

struct room* change_rooms(struct room* current_room, struct player* p) {
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
    printf("load Status: %i\n", status);
    status = unload_room(current_room);
    printf("Unload Status: %i\n", status);
    spawn_player(p->pos_x, SCREEN_HEIGHT - PLAYER_HEIGHT - 1, p);
    return current_room->north;
  } else if (p->pos_y == SCREEN_HEIGHT - PLAYER_HEIGHT && current_room->south) {
    status = load_room(current_room->south);
    printf("load Status: %i\n", status);
    status = unload_room(current_room);
    printf("Unload Status: %i\n", status);
    spawn_player(p->pos_x, 1, p);
    return current_room->south;
  } else {
      printf("Nowhere to go\n");
      return current_room;
  }
}

void link_rooms(int dir, struct room* r1, struct room* r2) {
  /*
  * The idea here is to choose a direction in which to link the rooms. The
  * direction is in reference to r1. I don't know if this is the permanent way I
  * want to set this up, but it will (hopefully) do for now.
  */
  switch(dir){
    case WEST:
      r1->west = r2;
      r2->east = r1;
      break;
    case EAST:
      r1->east = r2;
      r2->west = r1;
      break;
    case NORTH:
      r1->north = r2;
      r2->south = r1;
      break;
    case SOUTH:
      r1->south = r2;
      r2->north = r1;
      break;
  }
}

void show_room(struct room* r) {
    al_draw_bitmap(r->map, 0, 0, 0);
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
    generate_room(&r1, 1, "../assets/forest_1.png");
    generate_room(&r2, 2, "../assets/forest_2.png");
    generate_room(&r3, 3, "../assets/forest_3.png");
    generate_room(&r4, 4, "../assets/forest_4.png");
    generate_room(&r5, 5, "../assets/forest_5.png");
    generate_room(&r6, 6, "../assets/forest_6.png");
    generate_room(&r7, 7, "../assets/forest_7.png");
    generate_room(&r8, 8, "../assets/forest_8.png");
    generate_room(&r9, 9, "../assets/forest_9.png");

    link_rooms(WEST, &r1, &r2);
    link_rooms(NORTH, &r1, &r3);
    link_rooms(SOUTH, &r1, &r4);
    link_rooms(EAST, &r1, &r5);
    link_rooms(NORTH, &r2, &r6);
    link_rooms(SOUTH, &r2, &r7);
    link_rooms(WEST, &r3, &r6);
    link_rooms(EAST, &r3, &r8);
    link_rooms(NORTH, &r5, &r8);
    link_rooms(SOUTH, &r5, &r9);
    link_rooms(WEST, &r4, &r7);
    link_rooms(EAST, &r4, &r9);

    current_room = &r1;
    status += load_room(current_room);
    printf("Room pointer status: N %i, S %i, W %i, E %i.\n",r1.north->id, r1.south->id, r1.west->id, r1.east->id);
    if(status != OK) {
        printf("an Error has occured. Exiting...");
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
                  printf("current room ID: %i\n", current_room->id);
                }
                if(key[ALLEGRO_KEY_ESCAPE]) {
                    done = true;
                }
                if(key[ALLEGRO_KEY_T]) {
                  show_dev_tools = !show_dev_tools;
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
