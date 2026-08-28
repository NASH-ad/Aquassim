#include "collision_detection.h"
#include <math.h>

int check_collision_circle(rigidbody_t *body1, rigidbody_t *body2, float radius1, float radius2) {
    // Simple collision detection that checks if circles overlap
    vec2_t diff = vec2_sub(body1->position, body2->position);
    float distance = vec2_length(diff);
    float radius_sum = radius1 + radius2;

    return distance < radius_sum;
}

void resolve_collision_circle(rigidbody_t *a, rigidbody_t *b, float restitution, float mu) {
    // n : normal unit vector from b to a
    vec2_t n = vec2_norm(vec2_sub(a->position, b->position));
    vec2_t relative_velocity = vec2_sub(a->velocity, b->velocity);
    float vel_along_normal = vec2_dot(relative_velocity, n);

    if (vel_along_normal > 0) {
        return; // They are moving apart
    }

    float inv_sum_mass = a->inv_mass + b->inv_mass;
    if (inv_sum_mass == 0) {
        return; // Both objects are immovable
    }

    // Calculate and apply normal impulse
    float j = -(1.0f + restitution) * vel_along_normal / inv_sum_mass;
    vec2_t impulse = vec2_scale(n, j);
    a->velocity =  vec2_add(a->velocity, vec2_scale(impulse, a->inv_mass));
    b->velocity =  vec2_sub(b->velocity, vec2_scale(impulse, b->inv_mass));

    // Calculate and apply tangential impulse for friction
    vec2_t t_velocity = vec2_sub(relative_velocity, vec2_scale(n, vel_along_normal));
    float t_len = vec2_length(t_velocity);
    if (t_len < 1.0e-6f) {
        return; // No tangential velocity
    }
    vec2_t t = vec2_scale(t_velocity, 1.0f / t_len); // Normalize tangential velocity
    float jt = -vec2_dot(relative_velocity, t) / inv_sum_mass;

    vec2_t friction_impulse;
    if (fabs(jt) <= j * mu) {
        friction_impulse = vec2_scale(t, jt); // Static friction, there is no sliding
    } else {
        friction_impulse = vec2_scale(t, -j * mu); // Kinetic friction, sliding occurs
    }
    a->velocity = vec2_add(a->velocity, vec2_scale(friction_impulse, a->inv_mass));
    b->velocity = vec2_sub(b->velocity, vec2_scale(friction_impulse, b->inv_mass));
}
