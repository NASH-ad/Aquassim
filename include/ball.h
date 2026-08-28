#ifndef BALL_H
    #define BALL_H
    #include "rigidbody.h"
    #include "vector.h"

typedef struct ball {
    rigidbody_t *body;
    float radius;
} ball_t;

ball_t ball_create(rigidbody_t body, float radius);
void ball_destroy(ball_t *ball);

#endif // BALL_H