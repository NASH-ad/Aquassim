#ifndef ECS_H
#define ECS_H
#include <stdint.h>
#include "vector.h"
#include <unistd.h>
#include <stdbool.h>

/**
 * @brief A structure representing an entity in the ECS.
 * An entity is identified by a unique ID and a generation.
 */
typedef struct { uint32_t id; uint32_t gen; } entity_t;

#define ECS_INVALID UINT32_MAX
#define NULL_ENTITY (entity_t){ ECS_INVALID, 0u }
#define MAX_ENTITIES 16484u

typedef struct {
    uint32_t generations[MAX_ENTITIES];
    uint32_t free_ids[MAX_ENTITIES];
    uint32_t free_count;
    uint32_t first_unused_id;
} entity_manager_t;

bool entity_equal(entity_t a, entity_t b);

void em_init(entity_manager_t* em);
entity_t em_create(entity_manager_t* em);
void em_destroy(entity_manager_t* em, entity_t entity);
bool em_alive(const entity_manager_t* em, entity_t entity);

/**
 * @brief A structure representing a pool of components.
 * @param sparse_set: An array that maps entity indices to dense set indices. sparse_set[entity_index] = dense_index
 * @param dense_set: An array that stores the entity indices of the components in the pool. dense_set[dense_index] = entity_index
 * @param data: A contiguous block of memory that stores the actual component data.
 * @param count: The number of components currently in the pool.
 * @param capacity: The maximum number of components that can be stored in the pool.
 * @param elem_size: The size of each component in bytes.
 */
typedef struct {
    uint32_t sparse_set[MAX_ENTITIES];
    uint32_t dense_set[MAX_ENTITIES];
    char *data;
    uint32_t count;
    uint32_t capacity;
    uint32_t elem_size;
} pool_t;

void pool_init(pool_t *pool, uint32_t elem_size, uint32_t capacity);
void pool_destroy(pool_t *pool);
void *pool_add(pool_t *pool, uint32_t entity_index); // Returns pointer to the added uninitialised component
void *pool_get(pool_t *pool, uint32_t entity_index);
bool pool_has(pool_t *pool, uint32_t entity_index);
void pool_remove(pool_t *pool, uint32_t entity_index);

// Direct access to the dense array of components by index, without checking for existence
void *pool_at(pool_t *pool, uint32_t dense_index);

#endif // ECS_H