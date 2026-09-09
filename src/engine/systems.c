#include "simulator.h"
#include "ecs.h"
#include "collision_detection.h"
#include "creature.h"
#include <math.h>

/*
Integrate the positions of entities based on their velocities and inverse masses.
This function iterates over all entities in the position and velocity pools, updating their positions.
*/
void system_integrate(simulator_t *sim) {
    for (uint32_t i = 0; i < sim->position_pool.count; i++) {
        uint32_t entity_index = sim->position_pool.dense_set[i];
        vec2_t *pos = (vec2_t *)pool_at(&(sim->position_pool), entity_index);
        vec2_t *vel = (vec2_t *)pool_at(&(sim->velocity_pool), entity_index);
        vec2_t *prev_pos = (vec2_t *)pool_at(&(sim->prev_pos_pool), entity_index);
        if (!pos || !vel || !prev_pos) {
            continue;
        }
        *prev_pos = *pos;
        *pos = vec2_add(*pos, vec2_scale(*vel, sim->delta_time));
    }
}

/*
Resolve collisions between entities based on their positions, velocities, and radius.
This function checks for collisions between all pairs of entities in the radius pool
and resolves them using the provided restitution and friction coefficients.
*/
void system_resolve_collision(pool_t *radius_pool, pool_t *position_pool, pool_t *velocity_pool, pool_t *invmass_pool, float restitution, float friction) {
    for (uint32_t i = 0; i < radius_pool->count; ++i) {
        uint32_t entity_a_index = radius_pool->dense_set[i];
        float *radius_a = (float *)pool_get(radius_pool, entity_a_index);
        vec2_t *position_a = (vec2_t *)pool_get(position_pool, entity_a_index);
        vec2_t *velocity_a = (vec2_t *)pool_get(velocity_pool, entity_a_index);
        float *invmass_a = (float *)pool_get(invmass_pool, entity_a_index);

        if (!radius_a || !position_a || !velocity_a || !invmass_a) {
            continue; // Skip if any component is missing
        }

        for (uint32_t j = i + 1; j < radius_pool->count; ++j) {
            uint32_t entity_b_index = radius_pool->dense_set[j];
            float *radius_b = (float *)pool_get(radius_pool, entity_b_index);
            vec2_t *position_b = (vec2_t *)pool_get(position_pool, entity_b_index);
            vec2_t *velocity_b = (vec2_t *)pool_get(velocity_pool, entity_b_index);
            float *invmass_b = (float *)pool_get(invmass_pool, entity_b_index);

            if (!radius_b || !position_b || !velocity_b || !invmass_b) {
                continue; // Skip if any component is missing
            }

            if (check_collision_circle(*position_a, *position_b, *radius_a, *radius_b)) {
                resolve_collision_circle(position_a, position_b, velocity_a, velocity_b, *invmass_a, *invmass_b, restitution, friction);
            }
        }
    }
}

/*
Draw circles on the window based on their positions and radius.
*/
void system_draw_circles(pool_t *radius_pool, pool_t *position_pool, sfCircleShape *circle, sfRenderWindow *window, int pixels_per_unit) {
    sfCircleShape_setFillColor(circle, sfColor_fromRGB(200, 50, 255));
    for (uint32_t i = 0; i < radius_pool->count; ++i) {
        uint32_t entity_index = radius_pool->dense_set[i];
        float *radius = (float *)pool_get(radius_pool, entity_index);
        vec2_t *position = (vec2_t *)pool_get(position_pool, entity_index);

        if (!radius || !position) {
            continue; // Skip if any component is missing
        }

        sfCircleShape_setRadius(circle, (*radius) * pixels_per_unit);
        sfCircleShape_setOrigin(circle, (sfVector2f){(*radius) * pixels_per_unit / 2.0f, (*radius) * pixels_per_unit / 2.0f});
        sfCircleShape_setPosition(circle, (sfVector2f){position->x * pixels_per_unit, position->y * pixels_per_unit});
        sfRenderWindow_drawCircleShape(window, circle, NULL);
    }
}

//————————————————————————————————————————————————————
//-------- Systems for creature management ---------
//————————————————————————————————————————————————————

// This systems update the length of the muscles to simulate contraction and relaxation over time.
void system_muscles(entity_manager_t *mass_manager, joint_pool_t *pool, float time) {
    joint_t *joint = NULL;
    float s = 0.0f;

    for (uint32_t i = 0; i < pool->count; i++) {
        joint = &(pool->data[i]);
        
        if (!em_alive(mass_manager, joint->m_a) || !em_alive(mass_manager, joint->m_b) || !joint->is_muscle) {
            continue;
        }
        s = sinf(2 * M_PI * joint->frequency * time + joint->phase);
        joint->current_rest = joint->rest_length * (1.0f + joint->amplitude * s);
    }
}

// PURGE JOINTS SYSTEM
// This system removes all joints associated with a dead mass entity.
void system_purge_joints(simulator_t *sim) {
    uint32_t i = 0;
    joint_t *joint = NULL;

    while (i < sim->joint_pool.count) {
        joint = &(sim->joint_pool.data[i]);
        if (!em_alive(&(sim->mass_manager), joint->m_a) || !em_alive(&(sim->mass_manager), joint->m_b)) {
            sim->joint_pool.data[i] = sim->joint_pool.data[sim->joint_pool.count - 1];
            sim->joint_pool.count--;
            continue; // Do not increment i, as we need to check the new joint at index i
        }
        i++;
    }
}

void system_solve_joints(simulator_t *sim, int iters) {
    for (int it =0; it < iters; i++) {
        for (uint32_t i = 0; i < sim->joint_pool.count; i++) {
            joint_t *joint = &(sim->joint_pool.data[i]);
            if (!em_alive(&(sim->mass_manager), joint->m_a) || !em_alive(&(sim->mass_manager), joint->m_b)) {
                continue;
            }
            vec2_t *pos_a = (vec2_t *)pool_get(&(sim->position_pool), joint->m_a.id);
            vec2_t *pos_b = (vec2_t *)pool_get(&(sim->position_pool), joint->m_b.id);
            float *invmass_a = (float *)pool_get(&(sim->invmass_pool), joint->m_a.id);
            float *invmass_b = (float *)pool_get(&(sim->invmass_pool), joint->m_b.id);
            if (!pos_a || !pos_b || !invmass_a || !invmass_b) {
                continue;
            }
            vec2_t delta = vec2_sub(*pos_b, *pos_a);
            float dist = vec2_length(delta);
            float wsum = *invmass_a + *invmass_b;
            if (dist < 1e-6f || wsum == 0.0f) {
                continue;
            }
            vec2_t dir = vec2_scale(delta, 1.0f / dist);
            vec2_t correction = vec2_scale(dir, (dist - joint->current_rest) / wsum);
            *pos_a = vec2_add(*pos_a, vec2_scale(correction, *invmass_a));
            *pos_b = vec2_sub(*pos_b, vec2_scale(correction, *invmass_b));
        }
    }
}

void system_derive_velocity(simulator_t *sim) {
    float inv_dt = 1.0f / sim->delta_time;

    for (uint32_t i = 0; i < sim->position_pool.count; i++) {
        uint32_t entity_index = sim->position_pool.dense_set[i];
        vec2_t *pos = (vec2_t *)pool_at(&(sim->position_pool), i);
        vec2_t *prev_pos = (vec2_t *)pool_get(&(sim->prev_pos_pool), entity_index);
        vec2_t *vel = (vec2_t *)pool_get(&(sim->velocity_pool), entity_index);

        if (!pos || !prev_pos || !vel) {
            continue;
        }

        *vel = vec2_scale(vec2_sub(*pos, *prev_pos), inv_dt);
    }
}

void system_fitness(simulator_t *sim) {
    creature_t *creature = NULL;
    vec2_t centroid = (vec2_t){0.0f, 0.0f};

    for (uint32_t i =0; i < sim->creature_pool.count; i++) {
        creature = (creature_t *)pool_at(&(sim->creature_pool), i);
        centroid = creature_centroid(creature, sim);
        creature->fitness = vec2_length(vec2_sub(centroid, creature->spawn_centroid));
    }
}

void system_drag(simulator_t *sim, float Cn, float Ct) {

}