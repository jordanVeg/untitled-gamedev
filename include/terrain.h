#include "collisions.h"
#include "player.h"
#include "global.h"

#ifndef INCLUDE_TERRAIN_H
  #define INCLUDE_TERRAIN_H

#define ID_SIZE         8
#define IMAGE_PATH_SIZE 23

#define DOOR_HEIGHT (2 * PLAYER_HEIGHT)
#define DOOR_WIDTH  32

#define MAX_ROWS 10
#define MAX_COLS 10

#define NUM_ROWS 3
#define NUM_COLS 3

extern int floor_num;

typedef enum {
  BASIC,
  KEY,
  EXIT,
  SHOP,
  EMPTY,
  CHALLENGE
} ROOM_TYPE;

typedef struct room {
    int width, height, row_pos, col_pos;
    char id[ID_SIZE];
    char path_to_map_image[IMAGE_PATH_SIZE];
    ALLEGRO_BITMAP* map;
    ALLEGRO_BITMAP* door;
    ROOM_TYPE type;
    bool is_initialized, is_loaded, is_spawnable;
    /*
    * I want to implement the doors in the following way:
    * door array: {north, south, east, west}, I dont think it makes
    * much sense to create door hitboxes that dont actually exist...
    */

    Hitbox north_door, south_door, east_door, west_door;
} Room;

typedef struct floor {
  int number;
  Room map[MAX_ROWS][MAX_COLS];
} Floor;

Room generate_room(int row_pos, int col_pos, char image_path[IMAGE_PATH_SIZE]);

int load_room(Room* r);

int unload_room(Room* r);

Room* change_rooms(Room map[MAX_ROWS][MAX_COLS], Room* current_room, Player* p);

void link_rooms(Room map[MAX_ROWS][MAX_COLS]);

void show_room(Room* r);

void print_floor(Floor* f);

void generate_floor(Floor* f, int start_row, int start_col);

#endif
