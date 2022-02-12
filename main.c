/* Standard Libraries */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* Allegro Libraries */
#include <allegro5/allegro5.h>      /* Base Allegro Include */
#include <allegro5/allegro_font.h>  /* Allegro Font library */
#include <allegro5/allegro_image.h> /* Allegro Image library */
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>

#define FPS 30.0
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

int main(int argc, char** argv)
{
    al_set_config_value(al_get_system_config(), "trace", "level", "debug");
    if(!al_init())
    {
        printf("couldn't initialize allegro\n");
        return 1;
    }

    if(!al_install_keyboard())
    {
        printf("couldn't initialize keyboard\n");
        return 1;
    }

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
    al_init_primitives_addon();
    if(!al_install_audio())
    {
        printf("couldn't initialize audio\n");
        return 1;
    }
    
    if(!al_init_image_addon())
    {
        printf("couldn't initialize image addon.\n");
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

            case ALLEGRO_EVENT_KEY_DOWN:
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Hello world!");
            al_draw_bitmap(forest, 0, 0, 0 );

            al_draw_filled_triangle(35, 350, 85, 375, 35, 400, al_map_rgb_f(0, 1, 0));
            al_draw_filled_rectangle(240, 260, 340, 340, al_map_rgba_f(0, 0, 0.5, 0.5));
            al_draw_circle(450, 370, 30, al_map_rgb_f(1, 0, 1), 2);
            al_draw_line(440, 110, 460, 210, al_map_rgb_f(1, 0, 0), 1);
            al_draw_line(500, 220, 570, 200, al_map_rgb_f(1, 1, 0), 1);

            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_bitmap(forest);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}