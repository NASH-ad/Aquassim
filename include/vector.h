#ifndef VECTOR_H
#define VECTOR_H

typedef struct vec2 {
    float x;
    float y;
} vec2_t;

vec2_t vec2_add(vec2_t a, vec2_t b);
vec2_t vec2_subtract(vec2_t a, vec2_t b);
vec2_t vec2_multiply(vec2_t a, vec2_t b);
vec2_t vec2_scale(vec2_t v, float scalar);
vec2_t vec2_normalize(vec2_t v);
float vec2_length(vec2_t v);

#endif // VECTOR_H