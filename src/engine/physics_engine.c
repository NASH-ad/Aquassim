#include "physics_engine.h"

physics_engine_t physics_engine_create() {
    physics_engine_t engine;
    engine.bodies = NULL;
    engine.body_count = 0;
    engine.body_capacity = 0;
    return engine;
}

void physics_engine_add_body(physics_engine_t *engine, rigidbody_t *body) {
    if (engine->body_count + 1 > engine->body_capacity) {
        engine->bodies = realloc(engine->bodies, (engine->body_count + 20) * sizeof(rigidbody_t*));
        engine->body_capacity = engine->body_count + 20;
    }
    if (engine->bodies == NULL) {
        fprintf(stderr, "\x1B[31mFailed to allocate memory for new rigidbody(%ld).\x1B[0m\n", engine->body_count + 1);
        exit(EXIT_FAILURE);
    }
    engine->bodies[engine->body_count] = body;
    engine->body_count++;
    return;
}

void physics_engine_update(physics_engine_t *engine, float delta_time) {
    for (size_t i = 0; i < engine->body_count; i++) {
        rigidbody_update(engine->bodies[i], delta_time);
    }
}
