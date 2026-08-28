// Vector definition and operations

#include <math.h>
#include "vector.h"

vec2_t vec2_add(vec2_t a, vec2_t b) {
    vec2_t result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

vec2_t vec2_subtract(vec2_t a, vec2_t b) {
    vec2_t result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

vec2_t vec2_multiply(vec2_t a, vec2_t b) {
    vec2_t result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    return result;
}

vec2_t vec2_scale(vec2_t v, float scalar) {
    vec2_t result;
    result.x = v.x * scalar;
    result.y = v.y * scalar;
    return result;
}

vec2_t vec2_normalize(vec2_t v) {
    float length = vec2_length(v);
    if (length == 0) {
        return (vec2_t){0, 0};
    }
    return vec2_scale(v, 1.0f / length);
}

float vec2_length(vec2_t v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}
