#ifndef SIMULATOR_H
    #define SIMULATOR_H
    #include "aquassim.h"
    #include "physics_engine.h"
    #include "vector.h"
    #include "ball.h"
    #include <SFML/Graphics.h>

typedef struct simulator {
    physics_engine_t engine;
    vec2_t *gravity;
    ball_t ball1;
    ball_t ball2;
    sfCircleShape *circle;
    sfRenderWindow *window;
    sfVideoMode mode;
    sfView *view;
    sfSprite *background;
    sfTexture *background_texture;
} simulator_t;


void init_simulator(simulator_t *sim);
void cleanup_simulator(simulator_t *sim);

#endif // SIMULATOR_H