/* Standard Libraries */
#include <stdio.h>
#include <stdbool.h>

/* Allegro Libraries */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_font.h>          /* Allegro Font library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */
#include <allegro5/allegro_primitives.h>    /* Allegro Primatives library */

#include "terrain.h"

const Room DEFAULT_ROOM = {
  -1, -1, -1, -1, -1,
  NULL,
  NULL, NULL,
  EMPTY,
  false, false, false,
  DEFAULT_HITBOX, DEFAULT_HITBOX, DEFAULT_HITBOX, DEFAULT_HITBOX
};

void generate_room(Room* r, int ID, int row_pos, int col_pos, char* image_path) {
    r->width = SCREEN_WIDTH;
    r->height = SCREEN_HEIGHT;
    r->row_pos = row_pos;
    r->col_pos = col_pos;
    r->type = BASIC;
    r->is_spawnable = false;
    r->is_loaded = false;
    r->id = ID;
    r->path_to_map_image = image_path;
    r->is_initialized = true;

    create_hitbox(&r->north_door, -1, -1, 0, 0);
    create_hitbox(&r->south_door, -1, -1, 0, 0);
    create_hitbox(&r->east_door, -1, -1, 0, 0);
    create_hitbox(&r->west_door, -1, -1, 0, 0);
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
    printf("Loaded Room %d\n", r->id);
    return OK;
  } else {
    printf("Room %d load error: Initialization Status: %d, Load Status: %d\n", r->id, r->is_initialized, r->is_loaded);
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

Room* change_rooms(Room map[MAX_ROWS][MAX_COLS], Room* current_room, Player* p) {
  /* TODO: implement exception handling via status */
  int status;
  int curr_row = current_room->row_pos;
  int curr_col = current_room->col_pos;

  if (is_collision(&p->hb, &current_room->west_door) && map[curr_row-1][curr_col].is_initialized) {
    status = load_room(&map[curr_row-1][curr_col]);
    status = unload_room(current_room);
    spawn_player(SCREEN_WIDTH-DOOR_WIDTH-PLAYER_WIDTH-1, p->pos_y, p);
    return &map[curr_row-1][curr_col];
  }
  else if (is_collision(&p->hb, &current_room->east_door) && map[curr_row+1][curr_col].is_initialized) {
    status = load_room(&map[curr_row+1][curr_col]);
    status = unload_room(current_room);
    spawn_player(1 + DOOR_WIDTH, p->pos_y, p);
    return &map[curr_row+1][curr_col];
  }
  else if (is_collision(&p->hb, &current_room->north_door) && map[curr_row][curr_col-1].is_initialized) {
    status = load_room(&map[curr_row][curr_col-1]);
    status = unload_room(current_room);
    spawn_player(p->pos_x, SCREEN_HEIGHT - PLAYER_HEIGHT - DOOR_WIDTH - 1, p);
    return &map[curr_row][curr_col-1];
  }
  /* TODO: something weird going on with the south door hitboxes. */
  else if (is_collision(&p->hb, &current_room->south_door) && map[curr_row][curr_col+1].is_initialized) {
    status = load_room(&map[curr_row][curr_col+1]);
    status = unload_room(current_room);
    spawn_player(p->pos_x, DOOR_WIDTH + 1, p);
    return &map[curr_row][curr_col+1];
  }
  else {
    printf("Nowhere to go\n");
    return current_room;
  }
}

void link_rooms(Room map[MAX_ROWS][MAX_COLS]){
  for(int i = 0; i < MAX_ROWS; ++i){
    for(int j = 0; j < MAX_COLS; ++j){
      /* North */
      if(map[i][j].is_initialized) {
        if(map[i][j-1].is_initialized && j >= 1){
          //printf("Current Room %d,%d: North Room Detected!\n", i, j);
          create_hitbox(&map[i][j].north_door, SCREEN_WIDTH/2 - DOOR_HEIGHT/2, 0, DOOR_HEIGHT, DOOR_WIDTH);
        }
        /* South */
        if(map[i][j+1].is_initialized && j <= MAX_COLS-1){
          //printf("Current Room %d,%d: South Room Detected!\n", i, j);
          create_hitbox(&map[i][j].south_door, SCREEN_WIDTH/2 - DOOR_HEIGHT/2, SCREEN_HEIGHT - DOOR_WIDTH, DOOR_HEIGHT, DOOR_WIDTH);
        }
        /* East */
        if(map[i+1][j].is_initialized && i <= MAX_ROWS - 1){
          //printf("Current Room %d,%d: East Room Detected!\n", i, j);
          create_hitbox(&map[i][j].east_door, SCREEN_WIDTH - DOOR_WIDTH, SCREEN_HEIGHT/2 - DOOR_HEIGHT/2, DOOR_WIDTH, DOOR_HEIGHT);
        }
        /* West */
        if(map[i-1][j].is_initialized && i >= 1){
          //printf("Current Room %d,%d: West Room Detected!\n", i, j);
          create_hitbox(&map[i][j].west_door, 0, SCREEN_HEIGHT/2 - DOOR_HEIGHT/2, DOOR_WIDTH, DOOR_HEIGHT);
        }
      }      
    }
  }
}

void show_room(Room* r) {
  al_draw_bitmap(r->map, 0, 0, 0);

  /* draw doors of the room as well.. */
  if(r->north_door.px != -1) {
    al_draw_rotated_bitmap(r->door, 0, DOOR_HEIGHT/2, SCREEN_WIDTH/2, 0, ALLEGRO_PI/2, 0);
  }
  if(r->south_door.px != -1) {
    al_draw_rotated_bitmap(r->door, 0, DOOR_HEIGHT/2, SCREEN_WIDTH/2, SCREEN_HEIGHT - DOOR_WIDTH, ALLEGRO_PI/2, ALLEGRO_FLIP_HORIZONTAL);
  }
  if(r->east_door.px != -1) {
    al_draw_bitmap(r->door, SCREEN_WIDTH - DOOR_WIDTH, SCREEN_HEIGHT/2 - DOOR_HEIGHT/2, ALLEGRO_FLIP_HORIZONTAL);
  }
  if(r->west_door.px != -1) {
    al_draw_bitmap(r->door, 0, SCREEN_HEIGHT/2 - DOOR_HEIGHT/2, 0);
  }
}

void print_floor(Floor* f) {
  for(int i = 0; i < MAX_ROWS; ++i){
    for(int j = 0; j < MAX_COLS; ++j){
      printf(" %d ", f->map[i][j].is_initialized);
    }
    printf("\n");
  }
}

void generate_floor(Floor* f, int rows, int cols) {
  /* Room functionality testing */
  for(int i = 0; i < MAX_ROWS; ++i){
    for(int j = 0; j < MAX_COLS; ++j){
      f->map[i][j] = DEFAULT_ROOM;
    }
  }
  Room r1, r2, r3, r4, r5, r6, r7, r8, r9;

  /* Generate all the rooms on the floor */
  generate_room(&r1, 1, 1, 1, "../assets/forest_1.png");
  generate_room(&r2, 2, 0, 1, "../assets/forest_2.png");
  generate_room(&r3, 3, 1, 0, "../assets/forest_3.png");
  generate_room(&r4, 4, 1, 2, "../assets/forest_4.png");
  generate_room(&r5, 5, 2, 1, "../assets/forest_5.png");
  generate_room(&r6, 6, 0, 0, "../assets/forest_6.png");
  generate_room(&r7, 7, 0, 2, "../assets/forest_7.png");
  generate_room(&r8, 8, 2, 0, "../assets/forest_8.png");
  generate_room(&r9, 9, 2, 2, "../assets/forest_9.png");

  /* Link all the rooms together */
  f->map[1][1] = r1;
  f->map[0][1] = r2;
  f->map[1][0] = r3;
  f->map[1][2] = r4;
  f->map[2][1] = r5;
  f->map[0][0] = r6;
  f->map[0][2] = r7;
  f->map[2][0] = r8;
  f->map[2][2] = r9;

  link_rooms(f->map);

  print_floor(f);
}
