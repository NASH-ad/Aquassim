#include "simulator.h"

void cleanup_simulator(simulator_t *sim) {
    ball_destroy(&(sim->ball1));
    ball_destroy(&(sim->ball2));
    sfTexture_destroy(sim->background_texture);
    sfSprite_destroy(sim->background);
    sfCircleShape_destroy(sim->circle);
    sfRenderWindow_destroy(sim->window);
}