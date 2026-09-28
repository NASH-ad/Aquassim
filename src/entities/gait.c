#include "gait.h"
#include <math.h>

// Position in the current cycle, in [0, 1). Uses simulation time so runs are reproducible.
float gait_phase(const gait_t *gait, float time) {
    if (gait->period <= 0.0f) {
        return 0.0f;
    }
    float phase = fmodf(time + gait->clock_offset, gait->period) / gait->period;
    if (phase < 0.0f) {
        phase += 1.0f; // fmodf keeps the sign of a negative offset
    }
    return phase;
}

// Target length of a muscle at the given phase, as a fraction of its rest length.
// Blends the two surrounding keyframes with a smoothstep, so the muscle slows down to a stop
// at each keyframe; the last keyframe loops back to the first.
float gait_sample(const gait_t *gait, uint32_t muscle, float phase) {
    uint32_t n_frames = gait->n_frames;

    if (n_frames == 0 || muscle >= MAX_MUSCLES_PER_CREATURE) {
        return 1.0f; // No controller: keep the rest length
    }
    if (n_frames > MAX_KEYFRAMES) {
        n_frames = MAX_KEYFRAMES;
    }
    float x = phase * (float)n_frames;
    uint32_t k0 = (uint32_t)x % n_frames;
    uint32_t k1 = (k0 + 1) % n_frames;
    float f = x - floorf(x);
    float s = f * f * (3.0f - 2.0f * f);
    float a = gait->targets[k0][muscle];
    float b = gait->targets[k1][muscle];
    return a + (b - a) * s;
}
