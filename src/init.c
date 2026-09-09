#include "simulator.h"

static genome_t create_sample_genome(float amplitude, float freq) {
    genome_t genome = {0};

    // Create a simple creature with 3 nodes and 2 links
    genome.node_count = 5;
    for (uint32_t i = 0; i < 5; i++) {
        genome.nodes[i].offset = (vec2_t){(float)i * 0.8f - 1.6f, (float)i * 0.8f + 1.6f};
        genome.nodes[i].radius = 5.0f;
        genome.nodes[i].invmass = 1.0f;
    }

    uint32_t k = 0;

    for (uint32_t i = 0; i + 1 < 5; i++) {
        genome.links[k] = (gene_link_t){
            .a = i,
            .b = i + 1,
            .is_muscle = false,
            .amplitude = 0,
            .frequency = 0,
            .phase = 0.0f};
        k++;
    }

    for (uint32_t i = 0; i + 2 < 5; i++) {
        genome.links[k] = (gene_link_t){
            .a = i,
            .b = i + 2,
            .is_muscle = true,
            .amplitude = amplitude,
            .frequency = freq,
            .phase = (float)i * 1.6f};
        k++;
    }
    genome.link_count = k;
    return genome;
}

void init_simulator(simulator_t *sim) {
    const int pixels_per_unit = 50;

    sim->clock = sfClock_create();
    sim->delta_time = 0.0f;
    sim->time = sfTime_Zero;

    em_init(&(sim->mass_manager));
    pool_init(&(sim->position_pool), sizeof(vec2_t), MAX_ENTITIES);
    pool_init(&(sim->prev_pos_pool), sizeof(vec2_t), MAX_ENTITIES);
    pool_init(&(sim->velocity_pool), sizeof(vec2_t), MAX_ENTITIES);
    pool_init(&(sim->invmass_pool), sizeof(float), MAX_ENTITIES);
    pool_init(&(sim->radius_pool), sizeof(float), MAX_ENTITIES);
    pool_init(&(sim->part_of_pool), sizeof(part_of_t), MAX_ENTITIES);
    em_init(&(sim->creature_manager));
    pool_init(&(sim->creature_pool), sizeof(creature_t), MAX_CREATURES);
    joint_pool_init(&(sim->joint_pool), MAX_ENTITIES);

    genome_t sample = create_sample_genome(0.35f, 1.5f);
    entity_t e = spawn_creature(sim, &sample, (vec2_t){0.0f, 0.0f});
    if (e.id == NULL_ENTITY.id) {
        LOG("[ERROR] Failed to spawn sample creature\n");
    } else {
        LOG("INFO: Sample creature spawned with entity ID: %u\n", e.id);
    }
    
    // Window and graphics initialization
    sim->circle = sfCircleShape_create();
    sfCircleShape_setRadius(sim->circle, 5.0f * pixels_per_unit);
    sfCircleShape_setFillColor(sim->circle, sfRed);

    sim->mode = (sfVideoMode){1280, 700, 32};
    sim->window = sfRenderWindow_create(sim->mode, "Aquassim", sfClose, NULL);
    sfRenderWindow_setFramerateLimit(sim->window, 60);
    sim->view = sfView_create();
    sfView_setCenter(sim->view, (sfVector2f){0.0f, 0.0f});
    sfView_setSize(sim->view, (sfVector2f){(float)sim->mode.width, (float)sim->mode.height});
    sfRenderWindow_setView(sim->window, sim->view);

    sim->background = sfSprite_create();
    sim->background_texture = sfTexture_createFromFile("assets/Background.png", NULL);
    sfSprite_setTexture(sim->background, sim->background_texture, sfTrue);
    sfSprite_setPosition(sim->background, (sfVector2f){-(float)sim->mode.width / 2.0f, -(float)sim->mode.height / 2.0f});
}
