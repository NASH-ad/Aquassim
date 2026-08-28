// Main

#include "aquassim.h"
#include "collision_detection.h"
#include "physics_engine.h"
#include "rigidbody.h"
#include "vector.h"
#include "ball.h"
#include <SFML/Graphics.h>

int main(void) {
    const int pixels_per_unit = 50;
    physics_engine_t engine = physics_engine_create();
    ball_t ball1 = ball_create(rigidbody_create((vec2_t){-5.0f, 0.0f}, (vec2_t){2.0f, 0.0f}, 1.0f), 2.0f);
    ball_t ball2 = ball_create(rigidbody_create((vec2_t){10.0f, 1.0f}, (vec2_t){-5.0f, 0.0f}, 600.0f), 2.0f);

    physics_engine_add_body(&engine, ball1.body);
    physics_engine_add_body(&engine, ball2.body);

    // CFSML window creation
    sfVideoMode mode = {1280, 700, 32};
    sfRenderWindow *window = sfRenderWindow_create(mode, "Aquassim", sfClose, NULL);
    sfRenderWindow_setFramerateLimit(window, 60);
    sfEvent event;

    // Load background texture and create sprite
    sfSprite *background = sfSprite_create();
    sfTexture *background_texture = sfTexture_createFromFile("assets/Background.png", NULL); 
    sfSprite_setTexture(background, background_texture, sfTrue);

    // Create circle shapes to render balls
    sfCircleShape *circle1 = sfCircleShape_create();
    sfCircleShape_setRadius(circle1, ball1.radius * pixels_per_unit);
    sfCircleShape_setFillColor(circle1, sfRed);
    sfCircleShape_setOrigin(circle1, (sfVector2f){(ball1.radius * pixels_per_unit) / 2.0f, (ball1.radius * pixels_per_unit) / 2.0f});
    sfCircleShape *circle2 = sfCircleShape_create();
    sfCircleShape_setRadius(circle2, ball2.radius * pixels_per_unit);
    sfCircleShape_setFillColor(circle2, sfRed);
    sfCircleShape_setOrigin(circle2, (sfVector2f){(ball2.radius * pixels_per_unit) / 2.0f, (ball2.radius * pixels_per_unit) / 2.0f});

    const float delta_time = 0.016f;
    const vec2_t screen_center_offset = {(float)mode.width / 2.0f, (float)mode.height / 2.0f};
    int frame = 0;

    while (sfRenderWindow_isOpen(window)) {
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed) {
                sfRenderWindow_close(window);
            }
        }
        printf("\x1B[1;35mFrame %d: \x1B[0m\n", frame);
        physics_engine_update(&engine, delta_time);

        printf("Body 1 Position: (%f, %f)\n", ball1.body->position.x, ball1.body->position.y);
        printf("Body 2 Position: (%f, %f)\n", ball2.body->position.x, ball2.body->position.y);

        if (check_collision_circle(ball1.body, ball2.body, ball1.radius, ball2.radius)) {
            printf("Collision detected!\n");
            resolve_collision_circle(ball1.body, ball2.body, 5.0f);
        }
        frame++;

        sfRenderWindow_clear(window, sfBlack);
        sfRenderWindow_drawSprite(window, background, NULL);

        // Render balls
        sfCircleShape_setPosition(circle1, (sfVector2f){(ball1.body->position.x - ball1.radius) * pixels_per_unit + screen_center_offset.x,
            (ball1.body->position.y - ball1.radius) * pixels_per_unit + screen_center_offset.y});
        sfRenderWindow_drawCircleShape(window, circle1, NULL);

        sfCircleShape_setPosition(circle2, (sfVector2f){(ball2.body->position.x - ball2.radius) * pixels_per_unit + screen_center_offset.x,
            (ball2.body->position.y - ball2.radius) * pixels_per_unit + screen_center_offset.y});
        sfRenderWindow_drawCircleShape(window, circle2, NULL);
        

        sfRenderWindow_display(window);
    }

    // Ressource cleaning
    ball_destroy(&ball1);
    ball_destroy(&ball2);
    sfTexture_destroy(background_texture);
    sfSprite_destroy(background);
    sfCircleShape_destroy(circle1);
    sfCircleShape_destroy(circle2);
    sfRenderWindow_destroy(window);
    return 0;
}
