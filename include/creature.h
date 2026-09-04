#ifndef CREATURE_H
    #define CREATURE_H

    #include "ecs.h"
    #include "vector.h"

    #define MAX_MASSES_PER_CREATURE 20u
    #define MAX_JOINTS_PER_CREATURE 20u
    #define MAX_CREATURES 1000u

// A creature's mass in its genome
typedef struct {
    vec2_t offset;
    float radius;
    float invmass;
} gene_node_t;

// A creature's joint in its genome
typedef struct {
    uint32_t a;
    uint32_t b;
    bool is_muscle;
    float amplitude;
    float frequency;
    float phase;
} gene_link_t;

typedef struct {
    gene_node_t nodes[MAX_MASSES_PER_CREATURE]; // Array of nodes (masses) in the creature's genome
    uint32_t node_count;
    gene_link_t links[MAX_JOINTS_PER_CREATURE]; // Array of links (joints) in the creature's genome
    uint32_t link_count;
} genome_t;

// A creature is a collection of masses and joints that can move and interact with the environment.
// It moves by applying forces to its masses when it contracts its muscles.
typedef struct creature {
    genome_t genome;
    float fitness;
    float energy;
    entity_t masses[MAX_MASSES_PER_CREATURE];
    uint32_t mass_count;
} creature_t;

typedef struct {
    entity_t creature;
    entity_t m_a;
    entity_t m_b;
    float rest_length; // Rest length of the joint
    bool is_muscle; // Flag to indicate if the joint is a muscle
    float amplitude;  // Amplitude of the muscle contraction
    float frequency; // Frequency of the muscle contraction
    float phase;    // Phase of the muscle contraction
} joint_t;

// A joint is a link between two masses, it can act as a simple member or a muscle.
typedef struct {
    joint_t *data;
    uint32_t count;
    uint32_t capacity;
} joint_pool_t;

#endif // CREATURE_H