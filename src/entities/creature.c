#include "creature.h"
#include "simulator.h"
#include "aquassim.h"

/*
------- Joint pool handling ---------
*/
void joint_pool_init(joint_pool_t *pool, uint32_t capacity) {
    pool->data = (joint_t *)malloc(sizeof(joint_t) * capacity);
    if (!pool->data) {
        LOG("[ERROR] Failed to allocate memory for joint pool\n");
        pool->count = 0;
        pool->capacity = 0;
        return;
    }
    pool->count = 0;
    pool->capacity = capacity;
}

joint_t *joint_pool_add(joint_pool_t *pool) {
    if (pool->count >= pool->capacity) {
        LOG("[ERROR] Joint pool is full, cannot add more joints\n");
        return NULL;
    }
    joint_t *joint = &(pool->data[pool->count]);
    pool->count++;
    return joint;
}

void joint_pool_remove(joint_pool_t *pool, entity_t creature) {
    for (uint32_t i = 0; i < pool->count; i++) {
        if (pool->data[i].creature.id == creature.id) {
            // Move the last joint to the current position to fill the gap
            pool->data[i] = pool->data[pool->count - 1];
            pool->count--;
            return;
        }
    }
}

void joint_pool_destroy(joint_pool_t *pool) {
    if (pool->data) {
        free(pool->data);
        pool->data = NULL;
    }
    pool->count = 0;
    pool->capacity = 0;
}

/*
-------- Creature spawning ---------
*/
entity_t spawn_creature(simulator_t *sim, const genome_t *genome, vec2_t origin) {
    creature_t *creature = NULL;
    entity_t creature_entity = em_create(&(sim->creature_manager));
    
    if (creature_entity.id == NULL_ENTITY.id) {
        return NULL_ENTITY; // Return an invalid entity if creation failed
    }
    creature = (creature_t *)pool_get(&(sim->creature_pool), creature_entity.id);
    if (!creature) {
        return NULL_ENTITY; // Return an invalid entity if retrieval failed
    }

    // Initialize the creature's genome and other properties
    creature->genome = *genome;
    creature->fitness = 0.0f;
    creature->energy = 0.0f;
    creature->mass_count = 0;

    // Spawn masses based on the genome
    for (uint32_t i = 0; i < genome->node_count; i++) {
        gene_node_t *node = &(genome->nodes[i]);
        entity_t mass_entity = em_create(&(sim->mass_manager));
        float *radius = NULL;
        float *invmass = NULL;
        vec2_t *pos = NULL;
        part_of_t *part_of = NULL;
        
        if (mass_entity.id == NULL_ENTITY.id) {
            continue;
        }
        radius = (float *)pool_add(&(sim->radius_pool), mass_entity.id);
        invmass = (float *)pool_add(&(sim->invmass_pool), mass_entity.id);
        pos = (vec2_t *)pool_add(&(sim->position_pool), mass_entity.id);
        part_of = (part_of_t *)pool_add(&(sim->part_of_pool), mass_entity.id);
        if (!radius || !invmass || !pos || !part_of) {
            LOG("[ERROR] Failed to allocate component for mass entity\n");
            em_destroy(&(sim->mass_manager), mass_entity);
            continue;
        }
        *radius = node->radius;
        *invmass = node->invmass;
        *pos = vec2_add(origin, node->offset);
        creature->masses[creature->mass_count] = mass_entity;
        creature->mass_count++;
        *part_of = (part_of_t){.creature = creature_entity};
    }

    // Initialize the creature's joint pool
    for (uint32_t i = 0; i < genome->link_count; i++) {
        const gene_link_t *link = &(genome->links[i]);
        entity_t a = creature->masses[link->a];
        entity_t b = creature->masses[link->b];
        joint_t *joint = NULL;
        vec2_t *pos_a = (vec2_t *)pool_get(&(sim->position_pool), a.id);
        vec2_t *pos_b = (vec2_t *)pool_get(&(sim->position_pool), b.id);
        float rest_length = 0.0f;
        
        if (a.id == NULL_ENTITY.id || b.id == NULL_ENTITY.id) {
            LOG("[ERROR] Invalid mass entity for joint\n");
            continue;
        }
        rest_length = vec2_length(vec2_sub(*pos_a, *pos_b));
        joint = joint_pool_add(&(sim->joint_pool));
        if (!joint) {
            LOG("[ERROR] Failed to add joint to the pool\n");
            continue;
        }
        joint->creature = creature_entity;
        joint->m_a = a;
        joint->m_b = b;
        joint->rest_length = rest_length;
        joint->is_muscle = link->is_muscle;
        joint->amplitude = link->amplitude;
        joint->frequency = link->frequency;
        joint->phase = link->phase;
    }

    return creature_entity;
}