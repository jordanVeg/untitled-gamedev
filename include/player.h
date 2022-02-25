
#define PLAYER_WIDTH  64
#define PLAYER_HEIGHT 64
#define PLAYER_SPEED  16
#define OK     0
#define ERROR -1

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
} Player;

int initialize_player(Player* p);

int spawn_player(int start_x, int start_y, Player* p);

void update_player(unsigned char key[], struct player* p);

void show_player(struct player* p);
