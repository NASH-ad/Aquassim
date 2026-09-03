#include "collision_detection.h"
#include <math.h>

int check_collision_circle(vec2_t v1, vec2_t v2, float radius1, float radius2) {
    // Simple collision detection that checks if circles overlap
    vec2_t diff = vec2_sub(v1, v2);
    float distance = vec2_length(diff);
    float radius_sum = radius1 + radius2;

    return distance < radius_sum;
}

void resolve_collision_circle(vec2_t *pos_a, vec2_t *pos_b, vec2_t *vel_a, vec2_t *vel_b, float inv_mass_a, float inv_mass_b, float restitution, float mu) {
    // n : normal unit vector from b to a
    vec2_t n = vec2_norm(vec2_sub(*pos_a, *pos_b));
    vec2_t relative_velocity = vec2_sub(*vel_a, *vel_b);
    float vel_along_normal = vec2_dot(relative_velocity, n);

    if (vel_along_normal > 0) {
        return; // They are moving apart
    }

    float inv_sum_mass = inv_mass_a + inv_mass_b;
    if (inv_sum_mass == 0) {
        return; // Both objects are immovable
    }

    // Calculate and apply normal impulse
    float j = -(1.0f + restitution) * vel_along_normal / inv_sum_mass;
    vec2_t impulse = vec2_scale(n, j);
    *vel_a =  vec2_add(*vel_a, vec2_scale(impulse, inv_mass_a));
    *vel_b =  vec2_sub(*vel_b, vec2_scale(impulse, inv_mass_b));

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
    *vel_a = vec2_add(*vel_a, vec2_scale(friction_impulse, inv_mass_a));
    *vel_b = vec2_sub(*vel_b, vec2_scale(friction_impulse, inv_mass_b));
}
