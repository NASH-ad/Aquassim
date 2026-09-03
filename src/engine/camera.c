#include "simulator.h"

void update_camera_view(sfSprite *background, sfView *view, float delta_time) {
    const float move_speed = 1000.0f;
    const float adjusted_move_speed = move_speed * delta_time;

    if (sfKeyboard_isKeyPressed(sfKeyUp)) {
        sfView_move(view, (sfVector2f){0.0f, -adjusted_move_speed});
        sfSprite_move(background, (sfVector2f){0.0f, -adjusted_move_speed});
    }
    if (sfKeyboard_isKeyPressed(sfKeyDown)) {
        sfView_move(view, (sfVector2f){0.0f, adjusted_move_speed});
        sfSprite_move(background, (sfVector2f){0.0f, adjusted_move_speed});
    }
    if (sfKeyboard_isKeyPressed(sfKeyLeft)) {
        sfView_move(view, (sfVector2f){-adjusted_move_speed, 0.0f});
        sfSprite_move(background, (sfVector2f){-adjusted_move_speed, 0.0f});
    }
    if (sfKeyboard_isKeyPressed(sfKeyRight)) {
        sfView_move(view, (sfVector2f){adjusted_move_speed, 0.0f});
        sfSprite_move(background, (sfVector2f){adjusted_move_speed, 0.0f});
    }

    if (sfKeyboard_isKeyPressed(sfKeyZ)) {
        sfView_zoom(view, 0.95f);
        sfSprite_scale(background, (sfVector2f){0.95f, 0.95f});
        sfVector2f v_size = sfView_getSize(view);
        sfVector2f v_pos = sfView_getCenter(view);
        sfSprite_setPosition(background, (sfVector2f){v_pos.x - (v_size.x / 2.0f), v_pos.y - (v_size.y / 2.0f)});

    }
    if (sfKeyboard_isKeyPressed(sfKeyZ) && sfKeyboard_isKeyPressed(sfKeyLShift)) {
        sfView_zoom(view, 1.15f);
        sfSprite_scale(background, (sfVector2f){1.15f, 1.15f});
        sfVector2f v_size = sfView_getSize(view);
        sfVector2f v_pos = sfView_getCenter(view);
        sfSprite_setPosition(background, (sfVector2f){v_pos.x - (v_size.x / 2.0f), v_pos.y - (v_size.y / 2.0f)});
    }
}