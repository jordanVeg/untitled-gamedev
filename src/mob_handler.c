#include "mob_handler.h"

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
*   3. Add fucntionality to easily loop through non-default mobs in the array
*/
/*
* Initialize handler mob array and mob count value.
*/
void initialize_handler(MOB_HANDLER* handler, int max_mobs) {
    for(int index = 0; index < ABSOLUTE_MAX_MOBS; index++) {
        initialize_mob(&handler->mobs[index], DEFAULT, -1);
    }
    handler->local_max_mobs = max_mobs;
    handler->mob_count      = 0;
}

/*
* add a mob to the handler's array in the next available slot, increment 
* mob_count. Return OK if it was added successfully, ERROR otherwise
*/
int add_mob(MOB_HANDLER* handler, Mob mob) {
    for(int index = 0; index < handler->local_max_mobs; index++) {
        if(handler->mobs[index].type == DEFAULT) {
            handler->mobs[index] = mob;
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
int remove_mob(MOB_HANDLER* handler, Mob mob) {    
    for(int index = 0; index < handler->local_max_mobs; index++) {
        if(handler->mobs[index].id == mob.id) {
            initialize_mob(&handler->mobs[index], DEFAULT, -1);
            handler->mob_count--;
            return OK;
        }
    }
    return ERROR;
}
