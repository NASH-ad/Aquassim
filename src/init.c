#include "simulator.h"

void init_simulator(simulator_t *sim) {
    const int pixels_per_unit = 50;
    sim->engine = physics_engine_create();
    sim->ball1 = ball_create(rigidbody_create((vec2_t){-5.0f, 0.0f}, (vec2_t){2.0f, 0.0f}, 1.0f), 2.0f);
    sim->ball2 = ball_create(rigidbody_create((vec2_t){10.0f, 1.0f}, (vec2_t){-5.0f, 0.0f}, 600.0f), 2.0f);

    physics_engine_add_body(&(sim->engine), sim->ball1.body);
    physics_engine_add_body(&(sim->engine), sim->ball2.body);

    // CFSML window creation
    sim->mode = (sfVideoMode){1280, 700, 32};
    sim->window = sfRenderWindow_create(sim->mode, "Aquassim", sfClose, NULL);
    sfRenderWindow_setFramerateLimit(sim->window, 60);

    // Load background texture and create sprite
    sim->background = sfSprite_create();
    sim->background_texture = sfTexture_createFromFile("assets/Background.png", NULL); 
    sfSprite_setTexture(sim->background, sim->background_texture, sfTrue);

    // Create circle shapes to render balls
    sim->circle = sfCircleShape_create();
    sfCircleShape_setRadius(sim->circle, sim->ball1.radius * pixels_per_unit);
    sfCircleShape_setFillColor(sim->circle, sfRed);
    sfCircleShape_setOrigin(sim->circle, (sfVector2f){(sim->ball1.radius * pixels_per_unit) / 2.0f, (sim->ball1.radius * pixels_per_unit) / 2.0f});
}