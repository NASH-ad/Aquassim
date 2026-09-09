// Main

#include "aquassim.h"
#include "collision_detection.h"
#include "vector.h"
#include <SFML/Graphics.h>
#include "simulator.h"

int main(void) {
    const int pixels_per_unit = 50;
    simulator_t sim = {0};
    init_simulator(&sim);
    sfEvent event;

    //int frame = 0;

    while (sfRenderWindow_isOpen(sim.window)) {
        while (sfRenderWindow_pollEvent(sim.window, &event)) {
            if (event.type == sfEvtClosed) {
                sfRenderWindow_close(sim.window);
            }
        }
        sim.delta_time = sfTime_asSeconds(sfClock_restart(sim.clock));
        sim.time = sfSeconds(sim.delta_time + sfTime_asSeconds(sim.time));
        handle_input(&sim);
        
        //frame++;
        

        // Update simulation
        system_muscle(&(sim.mass_manager), &(sim.joint_pool), sfTime_asSeconds(sim.time));
        system_drag(&sim, 6.0f, 0.15f);
        system_purge_joints(&sim);
        system_integrate(&sim);
        system_solve_joints(&sim, 8);
        system_derive_velocity(&sim);
        system_fitness(&sim);

        sfRenderWindow_clear(sim.window, sfBlack);
        sfRenderWindow_setView(sim.window, sim.view);
        sfRenderWindow_drawSprite(sim.window, sim.background, NULL);

        // Render balls
        system_draw_joints(&sim, sim.window, pixels_per_unit);
        system_draw_circles(&(sim.radius_pool), &(sim.position_pool), sim.circle, sim.window, pixels_per_unit);

        sfRenderWindow_display(sim.window);
    }

    // Ressource cleaning
    cleanup_simulator(&sim);
    return 0;
}
