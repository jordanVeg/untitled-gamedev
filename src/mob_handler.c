#include <stdio.h>
#include <string.h>
#include "mob_handler.h"
#include "random.h"

MOB_HANDLER default_mob_handler() {
    MOB_HANDLER mob_handler = {
        .mobs = {default_mob()},
        .local_max_mobs = -1,
        .mob_count = -1
    };
    return mob_handler;
}
/*
* TODO: Generally, I might want to add in functionality to take care of the 
* following cases/tweak some things:
*   1. Trying to add a mob that happens to have the same ID as one already in
*      the handler.
*   2. I might want to compact the array as I remove mobs for ease of looping
*/
/*
* Initialize handler mob array and mob count value.
*/
void initialize_handler(MOB_HANDLER* handler, int max_mobs) {
    for(int index = 0; index < ABSOLUTE_MAX_MOBS; index++) {
        handler->mobs[index] = initialize_mob(DEFAULT, -1, -1, -1);
    }
    handler->local_max_mobs = max_mobs;
    handler->mob_count      = 0;
}
/*
* Reset mob count and clear mob array of a handler.
*/
void reset_handler(MOB_HANDLER* handler) {
    for(int index = 0; index < ABSOLUTE_MAX_MOBS; index++) {
        if(handler->mobs[index].type != DEFAULT) {
            /* reset mob */
            handler->mobs[index] = initialize_mob(DEFAULT, -1, -1, -1);
        }        
    }
    handler->mob_count = 0;
}

/*
* add a mob to the handler's array in the next available slot, increment 
* mob_count. Return OK if it was added successfully, ERROR otherwise
*/
int add_mob(MOB_HANDLER* handler, Mob mob) {
    for(int index = 0; index < handler->local_max_mobs; index++) {
        if(handler->mobs[index].type == DEFAULT) {
            printf("added mob to slot: %d\n", index);
            memcpy(&handler->mobs[index], &mob, sizeof(Mob));
            handler->mob_count++;
            return OK;
        }
    }
    return ERROR;
}

/*
* Remove a mob from the handler's array and decrement the counter. For now, I am
* leaving "holes" in the array, since I don't think it should matter, if a mob
* needs to be re-added, it will just fill the next available slot.
*/
int remove_mob(MOB_HANDLER* handler, Mob* mob) {    
    for(int index = 0; index < handler->local_max_mobs; index++) {
        if(handler->mobs[index].id == mob->id) {
            handler->mobs[index] = initialize_mob(DEFAULT, -1, -1, -1);
            handler->mob_count--;
            return OK;
        }
    }
    return ERROR;
}

/*
*  Update all active mobs in the mob array.
*/
void update_all_active_mobs(MOB_HANDLER* handler, int max_px, int max_py) {
    for(int index = 0; index < handler->local_max_mobs; index++) {
        if(handler->mobs[index].type != DEFAULT) {
            /* Update Mob State */
            handler->mobs[index].update(NULL, &handler->mobs[index], max_px, max_py);
            
            /* Check if mob died and remove them from the Array */
        }

    }
}

/*
*  Draw all active mobs in the mob array.
*/
void draw_all_active_mobs(MOB_HANDLER* handler, double delta_time) {
    for(int index = 0; index < handler->local_max_mobs; index++) {
        if(handler->mobs[index].type != DEFAULT) {
            handler->mobs[index].draw(&handler->mobs[index], delta_time);
        }
    }
}

void spawn_mobs(MOB_HANDLER* handler, int max_px, int max_py, int floor_number) {
    /* 
    * TODO: Create some sort of smart algorithm based on the floor number, and 
    * (when eventually implemented) a difficulty scalar using a point system to
    * create a very "dynamic" variety of mobs on a per-floor basis. For now tho,
    * Dumb and Quick!
    */
    int num_mobs = rng_random_int(1, 2*floor_number);
    int xpos, ypos;
    int offset = 32;
    for(int i = 0; i < num_mobs; i++) {
        xpos = rng_random_int(offset, (max_px - offset));
        ypos = rng_random_int(offset, (max_py - offset));
        Mob temp = initialize_mob(SLIME, i+1, xpos, ypos);
        add_mob(handler, temp);
    }
}
