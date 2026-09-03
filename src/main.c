// Main

#include "aquassim.h"
#include "collision_detection.h"
#include "vector.h"
#include <SFML/Graphics.h>
#include "simulator.h"

void handle_input(simulator_t *sim) {
    if (sfKeyboard_isKeyPressed(sfKeyEscape)) {
        sfRenderWindow_close(sim->window);
    }
    if (sfKeyboard_isKeyPressed(sfKeyUp)) {
        sfView_move(sim->view, (sfVector2f){0.0f, -10.0f});
        sfSprite_move(sim->background, (sfVector2f){0.0f, -10.0f});
    }
    if (sfKeyboard_isKeyPressed(sfKeyDown)) {
        sfView_move(sim->view, (sfVector2f){0.0f, 10.0f});
        sfSprite_move(sim->background, (sfVector2f){0.0f, 10.0f});
    }
    if (sfKeyboard_isKeyPressed(sfKeyLeft)) {
        sfView_move(sim->view, (sfVector2f){-10.0f, 0.0f});
        sfSprite_move(sim->background, (sfVector2f){-10.0f, 0.0f});
    }
    if (sfKeyboard_isKeyPressed(sfKeyRight)) {
        sfView_move(sim->view, (sfVector2f){10.0f, 0.0f});
        sfSprite_move(sim->background, (sfVector2f){10.0f, 0.0f});
    }
}

int main(void) {
    const int pixels_per_unit = 50;
    simulator_t sim = {0};
    init_simulator(&sim);
    sfEvent event;

    const float delta_time = 0.016f;
    int frame = 0;

    while (sfRenderWindow_isOpen(sim.window)) {
        while (sfRenderWindow_pollEvent(sim.window, &event)) {
            if (event.type == sfEvtClosed) {
                sfRenderWindow_close(sim.window);
            }
        }
        handle_input(&sim);
        
        frame++;

        // Update simulation
        system_integrate(&(sim.position_pool), &(sim.velocity_pool), &(sim.invmass_pool), delta_time);
        system_resolve_collision(&(sim.radius_pool), &(sim.position_pool), &(sim.velocity_pool), &(sim.invmass_pool), 0.8f, 0.5f);

        sfRenderWindow_clear(sim.window, sfBlack);
        sfRenderWindow_setView(sim.window, sim.view);
        sfRenderWindow_drawSprite(sim.window, sim.background, NULL);

        // Render balls
        system_draw_circles(&(sim.radius_pool), &(sim.position_pool), sim.circle, sim.window, pixels_per_unit);

        sfRenderWindow_display(sim.window);
    }

    // Ressource cleaning
    cleanup_simulator(&sim);
    return 0;
}
