#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <SFML/Graphics.h>
#include "vector.h"
#include <string.h>

typedef struct textbox {
    sfText *text;
    sfFont *font;
    sfVector2f topleftcorner_relative_pos;
    int size;
    sfColor text_color;
    char *content;
} textbox_t;

textbox_t textbox_create(sfText *text, sfFont *font, vec2_t topleftcorner_relative_pos, sfColor text_color, int size);
void textbox_display(sfRenderWindow *window, textbox_t *textbox);
void textbox_destroy(textbox_t *textbox);
void textbox_set_content(textbox_t *textbox, const char *content);

#endif // TEXTBOX_H
