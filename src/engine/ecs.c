#include "ecs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

bool entity_equal(entity_t a, entity_t b) {
    return a.id == b.id && a.gen == b.gen;
}

void em_init(entity_manager_t *em) {
    em->free_count = 0;
    em->first_unused_id = 0;
    for (uint32_t i = 0; i < MAX_ENTITIES; ++i) {
        em->generations[i] = 0;
        em->free_ids[i] = 0;
    }
}

entity_t em_create(entity_manager_t *em) {
    entity_t e;
    if (em->free_count > 0) {
        e.id = em->free_ids[em->free_count - 1];
        em->free_count--;
    } else {
        e.id = em->first_unused_id;
        em->first_unused_id += 1;
    }
    e.gen = em->generations[e.id];
    return e;
}

void em_destroy(entity_manager_t *em, entity_t entity) {
    if (entity.id < MAX_ENTITIES && em->generations[entity.id] == entity.gen) {
        em->generations[entity.id]++;
        em->free_ids[em->free_count] = entity.id;
        em->free_count++;
    }
}

bool em_alive(const entity_manager_t *em, entity_t entity) {
    return entity.id < em->first_unused_id && em->generations[entity.id] == entity.gen;
}

void pool_init(pool_t *pool, uint32_t elem_size, uint32_t capacity) {
    pool->elem_size = elem_size;
    pool->capacity = capacity;
    pool->count = 0;
    pool->data = (char *)malloc(elem_size * capacity);
    if (!pool->data) {
        // Handle memory allocation failure
        fprintf(stderr, "Failed to allocate memory for component pool\n");
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < MAX_ENTITIES; ++i) {
        pool->sparse_set[i] = ECS_INVALID; // Mark all as invalid
        pool->dense_set[i] = ECS_INVALID;  // Mark all as invalid
    }
}

void pool_destroy(pool_t *pool) {
    if (pool->data) {
        free(pool->data);
        pool->data = NULL;
    }
    for (uint32_t i = 0; i < MAX_ENTITIES; ++i) {
        pool->sparse_set[i] = ECS_INVALID;
        pool->dense_set[i] = ECS_INVALID;
    }
}

bool pool_has(pool_t *pool, uint32_t entity_index) {
    if (entity_index >= MAX_ENTITIES) {
        return false;
    }
    uint32_t dense_index = pool->sparse_set[entity_index];
    return (dense_index != ECS_INVALID && dense_index < pool->count);
}

void *pool_add(pool_t *pool, uint32_t entity_index) {
    if (entity_index >= MAX_ENTITIES || pool->count >= pool->capacity) {
        return NULL; // Invalid entity index or pool is full
    }
    if (pool_has(pool, entity_index)) {
        // Return existing component if it already exists
        return (void *)(pool->data + pool->sparse_set[entity_index] * pool->elem_size);
    }
    uint32_t dense_index = pool->count;
    pool->sparse_set[entity_index] = dense_index;
    pool->dense_set[dense_index] = entity_index;
    pool->count++;
    return (void *)(pool->data + dense_index * pool->elem_size);
}

void *pool_get(pool_t *pool, uint32_t entity_index) {
    if (!pool_has(pool, entity_index)) {
        return NULL; // Component does not exist for this entity
    }
    uint32_t dense_index = pool->sparse_set[entity_index];
    return (void *)(pool->data + dense_index * pool->elem_size);
}

void pool_remove(pool_t *pool, uint32_t entity_index) {
    if (!pool_has(pool, entity_index)) {
        return; // Component does not exist for this entity
    }
    uint32_t dense_index = pool->sparse_set[entity_index];
    uint32_t last_dense_index = pool->count - 1;
    if (dense_index != last_dense_index) {
        // Move the last component to the removed component's position
        uint32_t last_entity_index = pool->dense_set[last_dense_index];
        pool->dense_set[dense_index] = last_entity_index;
        pool->sparse_set[last_entity_index] = dense_index;
        memcpy(pool->data + dense_index * pool->elem_size,
               pool->data + last_dense_index * pool->elem_size,
               pool->elem_size);
    }
    pool->sparse_set[entity_index] = ECS_INVALID;
    pool->dense_set[last_dense_index] = ECS_INVALID;
    pool->count--;
}

void *pool_at(pool_t *pool, uint32_t dense_index) {
    if (dense_index >= pool->count) {
        return NULL; // Invalid dense index
    }
    return (void *)(pool->data + dense_index * pool->elem_size);
}