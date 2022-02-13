/* Standard Libraries */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* Allegro Libraries */
#include <allegro5/allegro5.h>              /* Base Allegro library */
#include <allegro5/allegro_font.h>          /* Allegro Font library */
#include <allegro5/allegro_image.h>         /* Allegro Image library */
#include <allegro5/allegro_primitives.h>    /* Allegro Primatives library */

#define FPS 30.0
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

#define PLAYER_WIDTH 64
#define PLAYER_HEIGHT 64
#define PLAYER_SPEED 64

typedef struct player
{
    int pos_x;
    int pos_y;
    int width;
    int height;
    int vel_x;
    int vel_y;
    int speed;
    ALLEGRO_BITMAP* sprite;
/*
    void (*initialize)(struct player* p);
    void (*spawn)(int start_x, int start_y, struct player* p);
    void (*update)(struct player* p);
*/
} Player;

int spawn_player(int start_x, int start_y, struct player* p)
{
    p->pos_x = start_x;
    p->pos_y = start_y;
    p->width = PLAYER_WIDTH;
    p->height = PLAYER_HEIGHT;
    p->vel_x = 0;
    p->vel_y = 0;
    p->speed = PLAYER_SPEED;
    p->sprite = al_load_bitmap("assets/wizard.png");
    if(!p->sprite)
    {
        printf("Error loading player sprite!\n");
        return 1;
    }

    return 0;
}

void update_player(ALLEGRO_KEYBOARD_EVENT keyboard, struct player* p)
{
    /* Update speed based on Button press */
    if(keyboard.keycode ==  ALLEGRO_KEY_W){
        //p->vel_y = -p->speed;
        p->pos_y -= p->speed;
    }
    if(keyboard.keycode ==  ALLEGRO_KEY_S){
        //p->vel_y = p->speed;
        p->pos_y += p->speed;
    }
    if(keyboard.keycode ==  ALLEGRO_KEY_A){
        //p->vel_x = -p->speed;
        p->pos_x -= p->speed;
    }
    if(keyboard.keycode ==  ALLEGRO_KEY_D){
        //p->vel_x = p->speed;
        p->pos_x += p->speed;
    }
    /* Update position based on speed */
    //p->pos_x += p->vel_x;
    //p->pos_y += p->vel_y;

    //p->vel_x = 0;
    //p->vel_y = 0;
}

void show_player(struct player* p)
{
    /* Draw player */
    al_draw_bitmap(p->sprite, p->pos_x, p->pos_y, 0 );
}

int main(int argc, char** argv)
{
//    al_set_config_value(al_get_system_config(), "trace", "level", "debug");

    /* Initialize everything */
    assert(al_init());                  /* Allegro environment */
    assert(al_install_keyboard());      /* Install Keyboard */
    assert(al_init_primitives_addon()); /* Initialize Primatives */
    assert(al_init_image_addon());      /* Initialize Image Library */

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    if(!timer)
    {
        printf("couldn't initialize timer\n");
        return 1;
    }

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    if(!queue)
    {
        printf("couldn't initialize queue\n");
        return 1;
    }    

    ALLEGRO_BITMAP* forest = al_load_bitmap("assets/Forest.jpg");
    if(!forest)
    {
        printf("couldn't load forest image.\n");
        return 1;
    }

    ALLEGRO_DISPLAY* disp = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if(!disp)
    {
        printf("couldn't initialize display\n");
        return 1;
    }

    ALLEGRO_FONT* font = al_create_builtin_font();
    if(!font)
    {
        printf("couldn't initialize font\n");
        return 1;
    }
    
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    /* Player Setup (hopefully) */
    Player p;
    int status = spawn_player(SCREEN_WIDTH/2 - PLAYER_WIDTH/2, SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2, &p);

    if(status != 0)
    {
        printf("an Error has occured. Exiting...");
        return 1;
    }

    /* Game Loop */
    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                // game logic goes here.
                redraw = true;
                break;

            case ALLEGRO_EVENT_KEY_CHAR:
                if(event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                {
                    done = true;
                    break;
                }
                update_player(event.keyboard, &p);
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_draw_bitmap(forest, 0, 0, 0);
            show_player(&p);
            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_bitmap(forest);
    al_destroy_bitmap(p.sprite);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}