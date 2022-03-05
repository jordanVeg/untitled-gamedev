/* Standard Libraries */
#include <stdio.h>

/* Allegro Libraries */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_font.h>          /* Allegro Font library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */
#include <allegro5/allegro_primitives.h>    /* Allegro Primatives library */

#include "terrain.h"

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
/*
    r->north_door = NULL;
    r->south_door = NULL;
    r->east_door = NULL;
    r->west_door = NULL;
*/
    create_hitbox(&r->north_door, 0, 0, 0, 0);
    create_hitbox(&r->south_door, 0, 0, 0, 0);
    create_hitbox(&r->east_door, 0, 0, 0, 0);
    create_hitbox(&r->west_door, 0, 0, 0, 0);

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
  if (is_collision(&p->hb, &current_room->west_door) && current_room->west) {
    status = load_room(current_room->west);
    status = unload_room(current_room);
    spawn_player(SCREEN_WIDTH-DOOR_WIDTH-PLAYER_WIDTH-1, p->pos_y, p);
    return current_room->west;
  }
  else if (is_collision(&p->hb, &current_room->east_door) && current_room->east) {
    status = load_room(current_room->east);
    status = unload_room(current_room);
    spawn_player(1 + DOOR_WIDTH, p->pos_y, p);
    return current_room->east;
  }
  else if (is_collision(&p->hb, &current_room->north_door) && current_room->north) {
    status = load_room(current_room->north);
    status = unload_room(current_room);
    spawn_player(p->pos_x, SCREEN_HEIGHT - PLAYER_HEIGHT - DOOR_WIDTH - 1, p);
    return current_room->north;
  }
  else if (is_collision(&p->hb, &current_room->south_door)&& current_room->south) {
    status = load_room(current_room->south);
    status = unload_room(current_room);
    spawn_player(p->pos_x, DOOR_WIDTH + 1, p);
    return current_room->south;
  }
  else {
    printf("Nowhere to go\n");
    return current_room;
  }
}

void link_rooms(Room* ref, Room* north, Room* south, Room* east, Room* west) {
  ref->north = north;
  ref->south = south;
  ref->east = east;
  ref->west = west;

  if(ref->west != NULL) {
    create_hitbox(&ref->west_door, 0, SCREEN_HEIGHT/2 - DOOR_HEIGHT/2, DOOR_WIDTH, DOOR_HEIGHT);
  }
  if(ref->east != NULL) {
    create_hitbox(&ref->east_door, SCREEN_WIDTH - DOOR_WIDTH, SCREEN_HEIGHT/2 - DOOR_HEIGHT/2, DOOR_WIDTH, DOOR_HEIGHT);
  }
  if(ref->north != NULL) {
    create_hitbox(&ref->north_door, SCREEN_WIDTH/2 - DOOR_HEIGHT/2, 0, DOOR_HEIGHT, DOOR_WIDTH);
  }
  if(ref->south != NULL) {
    create_hitbox(&ref->south_door, SCREEN_WIDTH/2 - DOOR_HEIGHT/2, SCREEN_HEIGHT - DOOR_WIDTH, DOOR_HEIGHT, DOOR_WIDTH);
  }

}

void show_room(Room* r) {
  al_draw_bitmap(r->map, 0, 0, 0);

  /* draw doors of the room as well.. */
  if(r->north != NULL) {
    al_draw_rotated_bitmap(r->door, 0, DOOR_HEIGHT/2, SCREEN_WIDTH/2, 0, ALLEGRO_PI/2, 0);
  }
  if(r->south != NULL) {
    al_draw_rotated_bitmap(r->door, 0, DOOR_HEIGHT/2, SCREEN_WIDTH/2, SCREEN_HEIGHT - DOOR_WIDTH, ALLEGRO_PI/2, ALLEGRO_FLIP_HORIZONTAL);
  }
  if(r->east != NULL) {
    al_draw_bitmap(r->door, SCREEN_WIDTH - DOOR_WIDTH, SCREEN_HEIGHT/2 - DOOR_HEIGHT/2, ALLEGRO_FLIP_HORIZONTAL);
  }
  if(r->west != NULL) {
    al_draw_bitmap(r->door, 0, SCREEN_HEIGHT/2 - DOOR_HEIGHT/2, 0);
  }
}
