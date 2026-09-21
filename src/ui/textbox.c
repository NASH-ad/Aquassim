#include "textbox.h"
#include <stdlib.h>

textbox_t textbox_create(sfText *text, sfFont *font, vec2_t topleftcorner_relative_pos, sfColor text_color, int size) {
    textbox_t textbox;
    textbox.text = text;
    textbox.font = font;
    textbox.topleftcorner_relative_pos = (sfVector2f){topleftcorner_relative_pos.x, topleftcorner_relative_pos.y};
    textbox.text_color = text_color;
    textbox.content = NULL;
    textbox.size = size;
    return textbox;
}

void textbox_display(sfRenderWindow *window, textbox_t *textbox) {
    if (textbox->content != NULL) {
        sfText_setString(textbox->text, textbox->content);
        sfText_setFont(textbox->text, textbox->font);
        sfText_setCharacterSize(textbox->text, textbox->size);
        sfText_setFillColor(textbox->text, textbox->text_color);

        const sfView *current_view = sfRenderWindow_getView(window);
        sfVector2f view_center = sfView_getCenter(current_view);
        sfVector2f view_size = sfView_getSize(current_view);
        sfVector2f view_top_left = {view_center.x - view_size.x / 2, view_center.y - view_size.y / 2};
        sfVector2f absolute_position = {view_top_left.x + textbox->topleftcorner_relative_pos.x,
            view_top_left.y + textbox->topleftcorner_relative_pos.y};
        sfText_setPosition(textbox->text, absolute_position);
        sfRenderWindow_drawText(window, textbox->text, NULL);
    }
}

void textbox_destroy(textbox_t *textbox) {
    if (textbox->content != NULL) {
        free(textbox->content);
        textbox->content = NULL;
    }
}

void textbox_set_content(textbox_t *textbox, const char *content) {
    if (textbox->content != NULL) {
        free(textbox->content);
    }
    textbox->content = strdup(content);
}