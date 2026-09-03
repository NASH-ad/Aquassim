#ifndef SIMULATOR_H
    #define SIMULATOR_H
    #include "aquassim.h"
    #include "physics_engine.h"
    #include "vector.h"
    #include "ball.h"
    #include "ecs.h"
    #include <SFML/Graphics.h>

typedef struct alt_simulator {
    physics_engine_t engine;
    vec2_t *gravity;
    sfCircleShape *circle;
    sfRenderWindow *window;
    sfVideoMode mode;
    sfView *view;
    sfSprite *background;
    sfTexture *background_texture;
    entity_manager_t entity_manager;
    pool_t position_pool;
    pool_t velocity_pool;
    pool_t invmass_pool;
    pool_t radius_pool;
} simulator_t;

void init_simulator(simulator_t *sim);
void cleanup_simulator(simulator_t *sim);

// Systems functions
void system_integrate(pool_t *position_pool, pool_t *velocity_pool, pool_t *invmass_pool, float delta_time);
void system_resolve_collision(pool_t *radius_pool, pool_t *position_pool, pool_t *velocity_pool, pool_t *invmass_pool, float restitution, float friction);
void system_draw_circles(pool_t *radius_pool, pool_t *position_pool, sfCircleShape *circle, sfRenderWindow *window, int pixels_per_unit);

#endif // SIMULATOR_H