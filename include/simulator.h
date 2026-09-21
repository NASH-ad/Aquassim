#ifndef SIMULATOR_H
    #define SIMULATOR_H
    #include "aquassim.h"
    #include "physics_engine.h"
    #include "vector.h"
    #include "ball.h"
    #include "ecs.h"
    #include <SFML/Graphics.h>
    #include "creature.h"
    #include "textbox.h"
    #include <string.h>


typedef struct part_of { entity_t creature; } part_of_t;

typedef struct alt_simulator {
    sfTime time;
    float delta_time;
    sfClock *clock;
    physics_engine_t engine;
    vec2_t *gravity;
    sfCircleShape *circle;
    sfRenderWindow *window;
    sfVideoMode mode;
    sfView *view;
    sfText *text;
    sfFont *font;
    textbox_t sim_infos;
    textbox_t creature_infos;
    sfSprite *background;
    sfTexture *background_texture;
    entity_manager_t creature_manager;
    pool_t creature_pool;
    entity_manager_t mass_manager;
    pool_t position_pool;
    pool_t prev_pos_pool;
    pool_t velocity_pool;
    pool_t invmass_pool;
    pool_t radius_pool;
    joint_pool_t joint_pool;
    pool_t part_of_pool;
} simulator_t;

void init_simulator(simulator_t *sim);
void cleanup_simulator(simulator_t *sim);

// Systems functions
void system_integrate(simulator_t *sim);
void system_resolve_collision(pool_t *radius_pool, pool_t *position_pool, pool_t *velocity_pool, pool_t *invmass_pool, float restitution, float friction);
void system_draw_circles(pool_t *radius_pool, pool_t *position_pool, sfCircleShape *circle, sfRenderWindow *window, int pixels_per_unit);
void system_draw_joints(simulator_t *sim, sfRenderWindow *window, int pixels_per_unit);

void system_muscle(entity_manager_t *mass_manager, joint_pool_t *pool, float time);
void system_purge_joints(simulator_t *sim);
void system_solve_joints(simulator_t *sim, int iters);
void system_derive_velocity(simulator_t *sim);
void system_fitness(simulator_t *sim);
void system_drag(simulator_t *sim, float Cn, float Ct);

// Utils
void handle_input(simulator_t *sim);
void update_camera_view(sfSprite *background, sfView *view, float delta_time);

entity_t spawn_creature(simulator_t *sim, const genome_t *genome, vec2_t origin);

#endif // SIMULATOR_H