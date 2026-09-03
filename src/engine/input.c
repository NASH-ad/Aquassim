#include "simulator.h"

void handle_input(simulator_t *sim) {
    if (sfKeyboard_isKeyPressed(sfKeyEscape)) {
        sfRenderWindow_close(sim->window);
    }
    update_camera_view(sim->background, sim->view, 0.016f);
}