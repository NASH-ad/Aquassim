// Ball enitity implementation

#include "ball.h"
#include "aquassim.h"

ball_t ball_create(rigidbody_t body, float radius) {
    ball_t ball;
    ball.body = malloc(sizeof(rigidbody_t));
    if (ball.body == NULL) {
        fprintf(stderr, "Failed to allocate memory for ball body.\n");
        exit(EXIT_FAILURE);
    }
    *ball.body = body;
    ball.radius = radius;
    return ball;
}

void ball_destroy(ball_t *ball) {
    if (ball->body != NULL) {
        free(ball->body);
        ball->body = NULL;
    }
}
