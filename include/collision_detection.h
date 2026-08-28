#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include "aquassim.h"
#include "rigidbody.h"
#include "vector.h"

int check_collision(rigidbody_t *body1, rigidbody_t *body2, float radius1, float radius2);

#endif // COLLISION_DETECTION_H