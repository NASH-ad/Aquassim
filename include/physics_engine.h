// Engine header file

#ifndef ENGINE_H
    #define ENGINE_H
    #include "aquassim.h"
    #include "rigidbody.h"
    #include "vector.h"

typedef struct physics_engine {
    rigidbody_t *bodies;
    size_t body_count;
    size_t body_capacity;
} physics_engine_t;

physics_engine_t physics_engine_create();
void physics_engine_add_body(physics_engine_t *engine, rigidbody_t body);
void physics_engine_update(physics_engine_t *engine, float delta_time);

#endif // ENGINE_H