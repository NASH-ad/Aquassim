// Main

#include "aquassim.h"
#include "collision_detection.h"
#include "physics_engine.h"
#include "rigidbody.h"
#include "vector.h"
#include "ball.h"

int main(void) {
    physics_engine_t engine = physics_engine_create();
    ball_t ball1 = ball_create(rigidbody_create((vec2_t){-5.0f, 0.0f}, (vec2_t){1.0f, 0.0f}, 1.0f), 1.0f);
    ball_t ball2 = ball_create(rigidbody_create((vec2_t){10.0f, 0.0f}, (vec2_t){-1.0f, 0.0f}, 1.0f), 1.0f);

    physics_engine_add_body(&engine, ball1.body);
    physics_engine_add_body(&engine, ball2.body);

    const float delta_time = 0.016f;

    for (int frame = 0; frame < 1000; frame++) {
        printf("\x1B[1;35mFrame %d: \x1B[0m\n", frame);
        physics_engine_update(&engine, delta_time);

        printf("Body 1 Position: (%f, %f)\n", ball1.body->position.x, ball1.body->position.y);
        printf("Body 2 Position: (%f, %f)\n", ball2.body->position.x, ball2.body->position.y);

        if (check_collision_circle(ball1.body, ball2.body, ball1.radius, ball2.radius)) {
            printf("Collision detected!\n");
            break;
        }
    }
    return 0;
}
