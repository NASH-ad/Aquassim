#include "creature.h"
#include "vector.h"
#include "simulator.h"

vec2_t creature_centroid(creature_t *creature, simulator_t *sim) {
    vec2_t centroid = (vec2_t){0.0f, 0.0f};
    uint32_t count = 0;

    for (uint32_t i = 0; i < creature->mass_count; i++) {
        entity_t mass = creature->masses[i];
        vec2_t *pos = (vec2_t *)pool_get(&(sim->position_pool), mass.id);
        if (!pos) {
            continue;
        }
        centroid = vec2_add(centroid, *pos);
        count++;
    }

    if (count > 0) {
        centroid = vec2_scale(centroid, 1.0f / (float)count);
    }

    return centroid;
}

entity_t spawn_creature(simulator_t *sim, const genome_t *genome, vec2_t origin) {
    entity_t creature_entity = em_create(&(sim->creature_manager));

    if (creature_entity.id == ECS_INVALID) {
        return NULL_ENTITY;
    }
    creature_t *creature = (creature_t *)pool_add(&(sim->creature_pool), creature_entity.id);
    if (!creature) {
        LOG("[ERROR] Failed to allocate creature in pool");
        return NULL_ENTITY;
    }
    creature->genome = *genome;
    creature->fitness = 0.0f;
    creature->energy = 0.0f;
    creature->mass_count = genome->node_count;
    creature->spawn_centroid = origin;

    for (uint32_t i = 0; i < genome->node_count; i++) {
        entity_t mass_entity = em_create(&(sim->mass_manager));
        if (mass_entity.id == ECS_INVALID) {
            LOG("[ERROR] Failed to create mass entity");
            return NULL_ENTITY;
        }
        uint32_t mass_index = mass_entity.id;
        vec2_t pos = vec2_add(origin, genome->nodes[i].offset);

        vec2_t *position = (vec2_t *)pool_add(&(sim->position_pool), mass_index);
        vec2_t *prev_position = (vec2_t *)pool_add(&(sim->prev_pos_pool), mass_index);
        vec2_t *velocity = (vec2_t *)pool_add(&(sim->velocity_pool), mass_index);
        float *invmass = (float *)pool_add(&(sim->invmass_pool), mass_index);
        float *radius = (float *)pool_add(&(sim->radius_pool), mass_index);
        part_of_t *part_of = (part_of_t *)pool_add(&(sim->part_of_pool), mass_index);

        if (!position || !prev_position || !velocity || !invmass || !radius || !part_of) {
            LOG("[ERROR] Failed to allocate components for mass entity");
            return NULL_ENTITY;
        }
        *position = pos;
        *prev_position = pos;
        *velocity = (vec2_t){0.0f, 0.0f};
        *invmass = genome->nodes[i].invmass;
        *radius = genome->nodes[i].radius;
        part_of->creature = creature_entity;
        
        creature->masses[i] = mass_entity;
    }

    for (uint32_t i = 0; i < genome->link_count; i++) {
        const gene_link_t *link = &(genome->links[i]);

        if (link->a >= creature->mass_count || link->b >= creature->mass_count) {
            LOG("[ERROR] Invalid link indices in genome");
            continue;
        }

        entity_t mass_a = creature->masses[link->a], mass_b = creature->masses[link->b];
        vec2_t *pos_a = (vec2_t *)pool_get(&(sim->position_pool), mass_a.id);
        vec2_t *pos_b = (vec2_t *)pool_get(&(sim->position_pool), mass_b.id);
        float L0 = vec2_length(vec2_sub(*pos_b, *pos_a));

        joint_t *joint = joint_pool_add(&(sim->joint_pool));
        if (!joint) {
            LOG("[ERROR] Failed to allocate joint in pool");
            continue;
        }
        joint->creature = creature_entity;
        joint->m_a = mass_a;
        joint->m_b = mass_b;
        joint->rest_length = L0;
        joint->current_rest = L0;
        joint->is_muscle = link->is_muscle;
        joint->amplitude = link->amplitude;
        joint->frequency = link->frequency;
        joint->phase = link->phase;
    }

    creature->spawn_centroid = creature_centroid(creature, sim);
    return creature_entity;
}
