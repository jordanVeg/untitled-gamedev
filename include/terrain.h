#ifndef INCLUDE_TERRAIN_H
#define INCLUDE_TERRAIN_H

#include "collisions.h"
#include "mob.h"
#include "global.h"
#include "mob_handler.h"

#define ID_SIZE             8
#define MAX_ROOM_WIDTH_IDX  20
#define MAX_ROOM_HEIGHT_IDX 15
#define PX_PER_TILE         64

#define DOOR_HEIGHT 128
#define DOOR_WIDTH  32

#define MAX_ROWS 20
#define MAX_COLS 20

extern int floor_num;

typedef enum room_type {
  R_BASIC,     //Normal enemy spawn
  R_CHALLENGE, //Challenge Item room
  R_DEFAULT,   //Default, no real room should have this type
  R_EXIT,      //Doorway to next floor
  R_HALLWAY,   //Hallway in between rooms, Need to figure out the best way to use
  R_KEY,       //Difficult Enemies, Key Room
  R_SHOP,      //Shop, No Enemy Spawn
  R_START,     //Starter Room, No Enemy Spawn
} Room_Type;

typedef struct room {
    int width, height, row_pos, col_pos;
    char id[ID_SIZE];
    int texture_map[MAX_ROOM_WIDTH_IDX][MAX_ROOM_HEIGHT_IDX];
    ALLEGRO_BITMAP* door;
    Room_Type type;
    bool is_initialized, is_loaded, is_spawnable, is_locked;
    int room_configuration[4];
    Hitbox north_door, south_door, east_door, west_door;
    Mob_Handler* m_handler_p;
} Room;

typedef struct floor {
  int number;
  int start_row;
  int stop_row;
  int start_col;
  int stop_col;
  ALLEGRO_BITMAP* texture_p;
  bool key_found;
  Room map[MAX_ROWS][MAX_COLS];
} Floor;

int load_room(Room* r);

int unload_room(Room* r);

Room* change_rooms(Room map[MAX_ROWS][MAX_COLS], Room* current_room, Mob* p);

void draw_room(Room* r, ALLEGRO_BITMAP* room_texture_p, double delta_time);

void generate_floor(Floor* f, int floor_num, int init_row, int init_col);

void destroy_floor(Floor* floor_p);

Room* update_dungeon_state(Floor* floor, Room* room, Mob* player);
/* Externally visible for debugging purposes*/
void print_floor(Floor* f);

#endif
