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

    const float delta_time = 0.016f;
    const vec2_t screen_center_offset = {(float)sim.mode.width / 2.0f, (float)sim.mode.height / 2.0f};
    int frame = 0;

    while (sfRenderWindow_isOpen(sim.window)) {
        while (sfRenderWindow_pollEvent(sim.window, &event)) {
            if (event.type == sfEvtClosed) {
                sfRenderWindow_close(sim.window);
            }
        }
        printf("\x1B[1;35mFrame %d: \x1B[0m\n", frame);
        physics_engine_update(&(sim.engine), delta_time);

        printf("Body 1 Position: (%f, %f)\n", sim.ball1.body->position.x, sim.ball1.body->position.y);
        printf("Body 2 Position: (%f, %f)\n", sim.ball2.body->position.x, sim.ball2.body->position.y);

        if (check_collision_circle(sim.ball1.body, sim.ball2.body, sim.ball1.radius, sim.ball2.radius)) {
            printf("Collision detected!\n");
            resolve_collision_circle(sim.ball1.body, sim.ball2.body, 1.0f, 0.5f); // restitution and friction coefficient
        }
        frame++;

        sfRenderWindow_clear(sim.window, sfBlack);
        sfRenderWindow_drawSprite(sim.window, sim.background, NULL);

        // Render balls
        sfCircleShape_setPosition(sim.circle, (sfVector2f){(sim.ball1.body->position.x - sim.ball1.radius) * pixels_per_unit + screen_center_offset.x,
            (sim.ball1.body->position.y - sim.ball1.radius) * pixels_per_unit + screen_center_offset.y});
        sfRenderWindow_drawCircleShape(sim.window, sim.circle, NULL);

        sfCircleShape_setPosition(sim.circle, (sfVector2f){(sim.ball2.body->position.x - sim.ball2.radius) * pixels_per_unit + screen_center_offset.x,
            (sim.ball2.body->position.y - sim.ball2.radius) * pixels_per_unit + screen_center_offset.y});
        sfRenderWindow_drawCircleShape(sim.window, sim.circle, NULL);
        

        sfRenderWindow_display(sim.window);
    }

    // Ressource cleaning
    cleanup_simulator(&sim);
    return 0;
}
