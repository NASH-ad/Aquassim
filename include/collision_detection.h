#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include "aquassim.h"
#include "rigidbody.h"
#include "vector.h"

int check_collision_circle(vec2_t v1, vec2_t v2, float radius1, float radius2);
void resolve_collision_circle(vec2_t *pos_a, vec2_t *pos_b, vec2_t *vel_a, vec2_t *vel_b, float inv_mass_a, float inv_mass_b, float restitution, float mu);

#endif // COLLISION_DETECTION_H