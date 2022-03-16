/* Standard Libraries */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* Allegro Libraries */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_font.h>          /* Allegro Font library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */
#include <allegro5/allegro_primitives.h>    /* Allegro Primatives library */

#include "terrain.h"

const Room DEFAULT_ROOM = {
  -1,             /* width */ 
  -1,             /* height */
  -1,             /* row position */
  -1,             /* column position */
  {""},           /* id string */
  {""},           /* background image path string */
  NULL,           /* ALLEGRO_BITMAP* map */
  NULL,           /* ALLEGRO_BITMAP* door */
  EMPTY,          /* room type */
  false,          /* is_initialized */
  false,          /* is_loaded */
  false,          /* is_spawnable */
  DEFAULT_HITBOX, /* north_door hitbox */
  DEFAULT_HITBOX, /* south_door hitbox */
  DEFAULT_HITBOX, /* east_door hitbox */
  DEFAULT_HITBOX  /* west_door hitbox */
};

Room generate_room(int row_pos, int col_pos, char image_path[IMAGE_PATH_SIZE]) {
  
    Room r = {
      .width   = SCREEN_WIDTH,
      .height  = SCREEN_HEIGHT,
      .row_pos = row_pos,
      .col_pos = col_pos,
      .map  = NULL,
      .door = NULL,
      .type = BASIC,
      .is_initialized = true,
      .is_loaded = false,
      .is_spawnable = false,
      .north_door = DEFAULT_HITBOX,
      .south_door = DEFAULT_HITBOX,
      .east_door = DEFAULT_HITBOX,
      .west_door = DEFAULT_HITBOX
    };

    /* generate id as row-col, always set to be 3 chars on each side of the dash */
    snprintf(r.id, ID_SIZE, "%03d-%03d", r.row_pos, r.col_pos);

    /* copy background image path */
    strncpy(r.path_to_map_image, image_path, IMAGE_PATH_SIZE);

    return r;
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
    printf("Loaded Room %s\n", r->id);
    return OK;
  } else {
    printf("Room %s load error: Initialization Status: %d, Load Status: %d\n", r->id, r->is_initialized, r->is_loaded);
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

  /* Check for north door collision */
  if(is_collision(&p->hb, &current_room->north_door) && map[curr_row][curr_col-1].is_initialized) {
    status = load_room(&map[curr_row][curr_col-1]);
    status = unload_room(current_room);
    spawn_player(p->pos_x, SCREEN_HEIGHT - PLAYER_HEIGHT - DOOR_WIDTH - 1, p);
    return &map[curr_row][curr_col-1];
  }
  /* Check for south door collision */
  else if(is_collision(&p->hb, &current_room->south_door) && map[curr_row][curr_col+1].is_initialized) {
    status = load_room(&map[curr_row][curr_col+1]);
    status = unload_room(current_room);
    spawn_player(p->pos_x, DOOR_WIDTH + 1, p);
    return &map[curr_row][curr_col+1];
  }
  /* Check for east door collision */
  else if(is_collision(&p->hb, &current_room->east_door) && map[curr_row+1][curr_col].is_initialized) {
    status = load_room(&map[curr_row+1][curr_col]);
    status = unload_room(current_room);
    spawn_player(1 + DOOR_WIDTH, p->pos_y, p);
    return &map[curr_row+1][curr_col];
  }
  /* Check for west door collision */
  else if(is_collision(&p->hb, &current_room->west_door) && map[curr_row-1][curr_col].is_initialized) {
    status = load_room(&map[curr_row-1][curr_col]);
    status = unload_room(current_room);
    spawn_player(SCREEN_WIDTH-DOOR_WIDTH-PLAYER_WIDTH-1, p->pos_y, p);
    return &map[curr_row-1][curr_col];
  }
  else {
    printf("Nowhere to go\n");
    return current_room;
  }
}

void link_rooms(Room map[MAX_ROWS][MAX_COLS]) {
  for(int i = 0; i < MAX_ROWS; ++i) {
    for(int j = 0; j < MAX_COLS; ++j) {
      /* Verify room is initialized before trying to create doorways */
      if(map[i][j].is_initialized) {
        /* North */
        if(map[i][j-1].is_initialized && j >= 1) {
          create_hitbox(&map[i][j].north_door, SCREEN_WIDTH/2 - DOOR_HEIGHT/2, 0, DOOR_HEIGHT, DOOR_WIDTH);
        }
        /* South */
        if(map[i][j+1].is_initialized && j <= MAX_COLS-1) {
          create_hitbox(&map[i][j].south_door, SCREEN_WIDTH/2 - DOOR_HEIGHT/2, SCREEN_HEIGHT - DOOR_WIDTH, DOOR_HEIGHT, DOOR_WIDTH);
        }
        /* East */
        if(map[i+1][j].is_initialized && i <= MAX_ROWS - 1) {
          create_hitbox(&map[i][j].east_door, SCREEN_WIDTH - DOOR_WIDTH, SCREEN_HEIGHT/2 - DOOR_HEIGHT/2, DOOR_WIDTH, DOOR_HEIGHT);
        }
        /* West */
        if(map[i-1][j].is_initialized && i >= 1) {
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
  /* Fill floor map with rooms:
  *  Current Algorithm is to create a square of rooms [rows]x[cols] large. 
  */

  for(int i = 0; i < MAX_ROWS; ++i) {
    for(int j = 0; j < MAX_COLS; ++j) {
      if( i <= rows && j <= cols) { 
        char room_path[IMAGE_PATH_SIZE];
        snprintf(room_path, sizeof(room_path), "../assets/forest_%d.png", (i+j)%9+1);
        f->map[i][j] = generate_room(i, j, room_path);
      } 
      else {
        f->map[i][j] = DEFAULT_ROOM;
      }
    }
  }

  link_rooms(f->map);
  print_floor(f);
}
