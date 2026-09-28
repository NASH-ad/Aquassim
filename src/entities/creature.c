#include "creature.h"
#include "simulator.h"
#include "aquassim.h"

/*
-------------------------------------
------- Joint pool handling ---------
-------------------------------------
*/

// Allocate memory for the internal array of joint_pool
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

// Add a new joint and return the pointer to set its properties
joint_t *joint_pool_add(joint_pool_t *pool) {
    if (pool->count >= pool->capacity) {
        LOG("[ERROR] Joint pool is full, cannot add more joints\n");
        return NULL;
    }
    joint_t *joint = &(pool->data[pool->count]);
    pool->count++;
    return joint;
}


// Remove every joint belonging to the given creature; doesn't free any memory
void joint_pool_remove(joint_pool_t *pool, entity_t creature) {
    uint32_t i = 0;

    while (i < pool->count) {
        if (entity_equal(pool->data[i].creature, creature)) {
            // Move the last joint to the current position to fill the gap
            pool->data[i] = pool->data[pool->count - 1];
            pool->count--;
            continue; // Do not increment i, as we need to check the new joint at index i
        }
        i++;
    }
}

// free the pool's memory
void joint_pool_destroy(joint_pool_t *pool) {
    if (pool->data) {
        free(pool->data);
        pool->data = NULL;
    }
    pool->count = 0;
    pool->capacity = 0;
}

/*
------------------------------------
-------- Creature spawning ---------
------------------------------------
*/

// Build a creature basing on the given genome and relative coordinatte system origin
// A creature is a collection of masses (articulations) and joints (members and muscles)
entity_t spawn_creature(simulator_t *sim, const genome_t *genome, vec2_t origin) {
    creature_t *creature = NULL;
    entity_t creature_entity = em_create(&(sim->creature_manager));
    
    if (creature_entity.id == NULL_ENTITY.id) {
        LOG("[ERROR] Failed to create creature entity\n");
        return NULL_ENTITY; // Return an invalid entity if creation failed
    }

    creature = (creature_t *)pool_add(&(sim->creature_pool), creature_entity.id);
    if (!creature) {
        LOG("[ERROR] Failed to allocate creature component\n");
        return NULL_ENTITY; // Return an invalid entity if retrieval failed
    }

    // Initialize the creature's genome and other properties
    creature->genome = *genome;
    creature->fitness = 0.0f;
    creature->energy = 0.0f;
    creature->mass_count = 0;

    // Spawn masses based on the genome
    for (uint32_t i = 0; i < genome->node_count; i++) {
        const gene_node_t *node = &(genome->nodes[i]);
        entity_t mass_entity = em_create(&(sim->mass_manager));
        float *radius = NULL;
        float *invmass = NULL;
        vec2_t *pos = NULL;
        vec2_t *prev_pos = NULL;
        part_of_t *part_of = NULL;
        vec2_t *vel = NULL;

        if (mass_entity.id == NULL_ENTITY.id) {
            LOG("[ERROR] Failed to create mass entity\n");
            continue;
        }
        radius = (float *)pool_add(&(sim->radius_pool), mass_entity.id);
        invmass = (float *)pool_add(&(sim->invmass_pool), mass_entity.id);
        pos = (vec2_t *)pool_add(&(sim->position_pool), mass_entity.id);
        prev_pos = (vec2_t *)pool_add(&(sim->prev_pos_pool), mass_entity.id);
        vel = (vec2_t *)pool_add(&(sim->velocity_pool), mass_entity.id);
        part_of = (part_of_t *)pool_add(&(sim->part_of_pool), mass_entity.id);
        if (!radius || !invmass || !pos || !prev_pos || !vel || !part_of) {
            LOG("[ERROR] Failed to allocate component for mass entity\n");
            // Remove the components that were added so no orphan is left behind
            pool_remove(&(sim->radius_pool), mass_entity.id);
            pool_remove(&(sim->invmass_pool), mass_entity.id);
            pool_remove(&(sim->position_pool), mass_entity.id);
            pool_remove(&(sim->prev_pos_pool), mass_entity.id);
            pool_remove(&(sim->velocity_pool), mass_entity.id);
            pool_remove(&(sim->part_of_pool), mass_entity.id);
            em_destroy(&(sim->mass_manager), mass_entity);
            continue;
        }
        *radius = node->radius;
        *invmass = node->invmass;
        *pos = vec2_add(origin, node->offset);
        *prev_pos = *pos;
        *vel = VEC2_NULL;
        creature->masses[creature->mass_count] = mass_entity;
        creature->mass_count++;
        *part_of = (part_of_t){.creature = creature_entity};
    }

    // Initialize the creature's joint pool
    uint32_t muscle_count = 0;
    for (uint32_t i = 0; i < genome->link_count; i++) {
        const gene_link_t *link = &(genome->links[i]);
        joint_t *joint = NULL;
        float rest_length = 0.0f;
        // Counted before any check so a muscle keeps its gait column even if an earlier link is skipped
        uint32_t muscle_id = link->is_muscle ? muscle_count++ : 0;

        // A mass may have failed to spawn, leaving fewer masses than genome nodes
        if (link->a >= creature->mass_count || link->b >= creature->mass_count) {
            LOG("[ERROR] Invalid mass entity for joint\n");
            continue;
        }
        entity_t a = creature->masses[link->a];
        entity_t b = creature->masses[link->b];
        vec2_t *pos_a = (vec2_t *)pool_get(&(sim->position_pool), a.id);
        vec2_t *pos_b = (vec2_t *)pool_get(&(sim->position_pool), b.id);
        if (!pos_a || !pos_b) {
            LOG("[ERROR] Missing position for joint masses\n");
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
        joint->current_rest = rest_length;
        joint->is_muscle = link->is_muscle;
        joint->muscle_id = muscle_id;
    }

    return creature_entity;
}


// The centroid is the creature's center of mass
vec2_t creature_centroid(creature_t *creature, pool_t *position_pool) {
    vec2_t centroid = (vec2_t){0.0f, 0.0f};
    uint32_t count = 0;

    for (uint32_t i = 0; i < creature->mass_count; i++) {
        entity_t mass = creature->masses[i];
        vec2_t *pos = (vec2_t *)pool_get(position_pool, mass.id);
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