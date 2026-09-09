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
