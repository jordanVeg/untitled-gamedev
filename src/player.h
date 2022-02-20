#include <stdlib.h>
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */

#define PLAYER_WIDTH  64
#define PLAYER_HEIGHT 64
#define PLAYER_SPEED  8

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

int spawn_player(int start_x, int start_y, struct player* p);

void update_player(unsigned char key[], struct player* p);

void show_player(struct player* p);
