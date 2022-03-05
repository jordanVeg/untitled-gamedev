#include "collisions.h"
#include "player.h"
#include "global.h"

#ifndef INCLUDE_TERRAIN_H
  #define INCLUDE_TERRAIN_H

#define DOOR_HEIGHT (2 * PLAYER_HEIGHT)
#define DOOR_WIDTH  32

typedef enum {
  BASIC,
  KEY,
  EXIT,
  SHOP,
  CHALLENGE
} ROOM_TYPE;

typedef struct room {
    int width, height, num_doors, id;
    char* path_to_map_image;
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

    struct room* north;
    struct room* south;
    struct room* east;
    struct room* west;
} Room;

void generate_room(Room* r, int ID, char* image_path);

int load_room(Room* r);

int unload_room(Room* r);

Room* change_rooms(Room* current_room, Player* p);

void link_rooms(Room* ref, Room* north, Room* south, Room* east, Room* west);

void show_room(Room* r);


#endif
