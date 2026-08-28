// Rigid Body Dynamics Implementation

#include "vector.h"
#include "rigidbody.h"

rigidbody_t rigidbody_create(vec2_t position, vec2_t velocity, float mass) {
    rigidbody_t rb;
    rb.position = position;
    rb.velocity = velocity;
    rb.mass = mass;
    rb.inv_mass = (mass != 0.0f) ? 1.0f / mass : 0.0f; // Avoid division by zero
    return rb;
}

void rigidbody_apply_force(rigidbody_t *rb, vec2_t force, float delta_time) {
    vec2_t acceleration = vec2_scale(force, 1.0f / rb->mass);
    rb->velocity = vec2_add(rb->velocity, vec2_scale(acceleration, delta_time));
}

void rigidbody_update(rigidbody_t *rb, float delta_time) {
    rb->position = vec2_add(rb->position, vec2_scale(rb->velocity, delta_time));
}