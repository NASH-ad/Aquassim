// Rigid Body Dynamics header file

#ifndef RIGIDBODY_H
#define RIGIDBODY_H
#include "vector.h"

typedef struct rigidbody {
    vec2_t position;
    vec2_t velocity;
    float mass;
} rigidbody_t;

rigidbody_t rigidbody_create(vec2_t position, vec2_t velocity, float mass);
void rigidbody_apply_force(rigidbody_t *rb, vec2_t force, float delta_time);
void rigidbody_update(rigidbody_t *rb, float delta_time);

#endif // RIGIDBODY_H