#ifndef GAIT_H
    #define GAIT_H
    #include <stdint.h>

    #define MAX_KEYFRAMES 6u
    #define MAX_MUSCLES_PER_CREATURE 20u // Must not exceed MAX_JOINTS_PER_CREATURE

// A gait is the creature's muscle controller: an internal clock reads a table of poses (keyframes).
// Each keyframe gives, for every muscle, the length it aims for as a fraction of its rest length
// (0.8 = contracted by 20%). Targets are written into the muscles' rest length, so the constraint
// solver does the actual work.
typedef struct {
    float period;       // Duration of one cycle, in seconds
    float clock_offset; // Shifts the clock so creatures don't all beat together, in seconds
    uint32_t n_frames;  // Number of keyframes in use, from 1 to MAX_KEYFRAMES
    float targets[MAX_KEYFRAMES][MAX_MUSCLES_PER_CREATURE]; // [keyframe][muscle], fraction of rest length
} gait_t;

float gait_phase(const gait_t *gait, float time);
float gait_sample(const gait_t *gait, uint32_t muscle, float phase);

#endif // GAIT_H
