#include "simulator.h"

void cleanup_simulator(simulator_t *sim) {
    joint_pool_destroy(&(sim->joint_pool));
    sfClock_destroy(sim->clock);
    pool_destroy(&(sim->position_pool));
    pool_destroy(&(sim->velocity_pool));
    pool_destroy(&(sim->invmass_pool));
    pool_destroy(&(sim->radius_pool));
    sfTexture_destroy(sim->background_texture);
    sfSprite_destroy(sim->background);
    sfCircleShape_destroy(sim->circle);
    sfRenderWindow_destroy(sim->window);
    sfView_destroy(sim->view);
}
