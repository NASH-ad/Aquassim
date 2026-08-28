// Main

#include "aquassim.h"
#include "collision_detection.h"
#include "physics_engine.h"
#include "rigidbody.h"
#include "vector.h"

int main(void) {
    physics_engine_t engine = physics_engine_create();

    rigidbody_t body1 = rigidbody_create((vec2_t){0.0f, 0.0f}, (vec2_t){1.0f, 1.0f}, 1.0f);
    rigidbody_t body2 = rigidbody_create((vec2_t){10.0f, 10.0f}, (vec2_t){-1.0f, -1.0f}, 1.0f);
    physics_engine_add_body(&engine, body1);
    physics_engine_add_body(&engine, body2);

    const float delta_time = 0.016f;

    for (int i = 0; i < 100; ++i) {
        physics_engine_update(&engine, delta_time);

        printf("Body 1 Position: (%f, %f)\n", body1.position.x, body1.position.y);
        printf("Body 2 Position: (%f, %f)\n", body2.position.x, body2.position.y);

        if (check_collision(&body1, &body2, 1.0f, 1.0f)) {
            printf("Collision detected!\n");
            break;
        }
    }
    return 0;
}
