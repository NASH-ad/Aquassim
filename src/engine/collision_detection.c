#include "collision_detection.h"

int check_collision(rigidbody_t *body1, rigidbody_t *body2, float radius1, float radius2) {
    // Simple collision detection that checks if circles overlap
    vec2_t diff = vec2_subtract(body1->position, body2->position);
    float distance = vec2_length(diff);
    float radius_sum = radius1 + radius2;

    return distance < radius_sum;
}