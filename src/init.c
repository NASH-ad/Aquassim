#include "simulator.h"

void init_simulator(simulator_t *sim) {
    const int pixels_per_unit = 50;

    sim->clock = sfClock_create();
    sim->delta_time = 0.0f;
    sim->time = sfTime_Zero;

    em_init(&(sim->mass_manager));
    pool_init(&(sim->position_pool), sizeof(vec2_t), MAX_ENTITIES);
    pool_init(&(sim->velocity_pool), sizeof(vec2_t), MAX_ENTITIES);
    pool_init(&(sim->invmass_pool), sizeof(float), MAX_ENTITIES);
    pool_init(&(sim->radius_pool), sizeof(float), MAX_ENTITIES);
    entity_t ball1 = em_create(&(sim->mass_manager));
    entity_t ball2 = em_create(&(sim->mass_manager));
    vec2_t *ball1_position = (vec2_t *)pool_add(&(sim->position_pool), ball1.id);
    vec2_t *ball1_velocity = (vec2_t *)pool_add(&(sim->velocity_pool), ball1.id);
    float *ball1_invmass = (float *)pool_add(&(sim->invmass_pool), ball1.id);
    float *ball1_radius = (float *)pool_add(&(sim->radius_pool), ball1.id);
    vec2_t *ball2_position = (vec2_t *)pool_add(&(sim->position_pool), ball2.id);
    vec2_t *ball2_velocity = (vec2_t *)pool_add(&(sim->velocity_pool), ball2.id);
    float *ball2_invmass = (float *)pool_add(&(sim->invmass_pool), ball2.id);
    float *ball2_radius = (float *)pool_add(&(sim->radius_pool), ball2.id);
    *ball1_position = (vec2_t){-5.0f, 0.0f};
    *ball1_velocity = (vec2_t){2.0f, 0.0f};
    *ball1_invmass = 1.0f;
    *ball1_radius = 2.0f;
    *ball2_position = (vec2_t){10.0f, 1.0f};
    *ball2_velocity = (vec2_t){-5.0f, 0.0f};
    *ball2_invmass = 600.0f;
    *ball2_radius = 2.0f;
    
    // Window and graphics initialization
    sim->circle = sfCircleShape_create();
    sfCircleShape_setRadius(sim->circle, (*ball1_radius) * pixels_per_unit);
    sfCircleShape_setFillColor(sim->circle, sfRed);

    sim->mode = (sfVideoMode){1280, 700, 32};
    sim->window = sfRenderWindow_create(sim->mode, "Aquassim", sfClose, NULL);
    sfRenderWindow_setFramerateLimit(sim->window, 60);
    sim->view = sfView_create();
    sfView_setCenter(sim->view, (sfVector2f){0.0f, 0.0f});
    sfView_setSize(sim->view, (sfVector2f){(float)sim->mode.width, (float)sim->mode.height});
    sfRenderWindow_setView(sim->window, sim->view);

    sim->background = sfSprite_create();
    sim->background_texture = sfTexture_createFromFile("assets/Background.png", NULL);
    sfSprite_setTexture(sim->background, sim->background_texture, sfTrue);
    sfSprite_setPosition(sim->background, (sfVector2f){-(float)sim->mode.width / 2.0f, -(float)sim->mode.height / 2.0f});
}
