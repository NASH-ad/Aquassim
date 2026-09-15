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
Draw circles on the window based on their positions and radius.
*/
void system_draw_circles(pool_t *radius_pool, pool_t *position_pool, sfCircleShape *circle, sfRenderWindow *window, int pixels_per_unit) {
    sfCircleShape_setFillColor(circle, sfColor_fromRGB(200, 50, 255));
    for (uint32_t i = 0; i < radius_pool->count; i++) {

        uint32_t entity_index = radius_pool->dense_set[i];
        float *radius = (float *)pool_get(radius_pool, entity_index);
        vec2_t *position = (vec2_t *)pool_get(position_pool, entity_index);

        if (!radius || !position) {
            continue; // Skip if any component is missing
        }

        sfCircleShape_setRadius(circle, (*radius));
        sfCircleShape_setOrigin(circle, (sfVector2f){(*radius) / 2.0f, (*radius) / 2.0f});
        sfCircleShape_setPosition(circle, (sfVector2f){position->x * pixels_per_unit, position->y * pixels_per_unit});
        sfRenderWindow_drawCircleShape(window, circle, NULL);
    }
}

void system_draw_joints(simulator_t *sim, sfRenderWindow *window, int pixels_per_unit) {
    for (uint32_t i = 0; i < sim->joint_pool.count; ++i) {

        joint_t *joint = &(sim->joint_pool.data[i]);
        if (!em_alive(&(sim->mass_manager), joint->m_a) || !em_alive(&(sim->mass_manager), joint->m_b) || joint->is_muscle) {
            continue;
        }

        vec2_t *pos_a = (vec2_t *)pool_get(&(sim->position_pool), joint->m_a.id);
        vec2_t *pos_b = (vec2_t *)pool_get(&(sim->position_pool), joint->m_b.id);

        if (!pos_a || !pos_b) {
            continue;
        }

        sfVertex a = (sfVertex){
            .position = {pos_a->x * pixels_per_unit, pos_a->y * pixels_per_unit},
            .color = sfColor_fromRGB(255, 255, 255),
            .texCoords = {0, 0},
        };

        sfVertex b = (sfVertex){
            .position = {pos_b->x * pixels_per_unit, pos_b->y * pixels_per_unit},
            .color = sfColor_fromRGB(255, 255, 255),
            .texCoords = {0, 0},
        };

        sfVertex line[] = {a, b};

        sfRenderWindow_drawPrimitives(window, line, 2, sfLines, NULL);
    }
}

//————————————————————————————————————————————————————
//-------- Systems for creature management ---------
//————————————————————————————————————————————————————

// This systems update the length of the muscles to simulate contraction and relaxation over time.
void system_muscle(entity_manager_t *mass_manager, joint_pool_t *pool, float time) {
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
    for (int it =0; it < iters; it++) {
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
        centroid = creature_centroid(creature, &(sim->position_pool));
        creature->fitness = vec2_length(vec2_sub(centroid, creature->spawn_centroid));
    }
}

void system_drag(simulator_t *sim, float Cn, float Ct) {
    joint_t *joint = NULL;

    for (uint32_t i = 0; i < sim->joint_pool.count; i++) {
        joint = &(sim->joint_pool.data[i]);
        if (!em_alive(&(sim->mass_manager), joint->m_a) || !em_alive(&(sim->mass_manager), joint->m_b)
            || !joint->is_muscle) {
            continue;
        }

        vec2_t *pos_a = (vec2_t *)pool_get(&(sim->position_pool), joint->m_a.id);
        vec2_t *pos_b = (vec2_t *)pool_get(&(sim->position_pool), joint->m_b.id);
        vec2_t *vel_a = (vec2_t *)pool_get(&(sim->velocity_pool), joint->m_a.id);
        vec2_t *vel_b = (vec2_t *)pool_get(&(sim->velocity_pool), joint->m_b.id);
        float *invmass_a = (float *)pool_get(&(sim->invmass_pool), joint->m_a.id);
        float *invmass_b = (float *)pool_get(&(sim->invmass_pool), joint->m_b.id);

        if (!pos_a || !pos_b || !vel_a || !vel_b || !invmass_a || !invmass_b) {
            continue;
        }

        vec2_t axis = vec2_sub(*pos_b, *pos_a);
        float length = vec2_length(axis);
        if (length < 1e-6f) {
            continue;
        }

        vec2_t tangential = vec2_scale(axis, 1.0f / length);
        vec2_t normal = (vec2_t){-tangential.y, tangential.x};

        vec2_t v = vec2_scale(vec2_add(*vel_a, *vel_b), 0.5f);
        float v_tangential = vec2_dot(v, tangential);
        float v_normal = vec2_dot(v, normal);

        // Implicit drag forces. We divide instead of minus because minus can cause instability
        float kn = Cn * fabsf(v_normal) * length * 0.5f;
        float kt = Ct * fabsf(v_tangential) * length * 0.5f;
        float vn_new = v_normal / (1.0f + kn * (*invmass_a + *invmass_b) * sim->delta_time);
        float vt_new = v_tangential / (1.0f + kt * (*invmass_a + *invmass_b) * sim->delta_time);

        vec2_t dv = vec2_add(vec2_scale(normal, vn_new - v_normal), vec2_scale(tangential, vt_new - v_tangential));
        *vel_a = vec2_add(*vel_a, dv);
        *vel_b = vec2_add(*vel_b, dv);
    }
}