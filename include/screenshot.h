#ifndef SCREENSHOT_H
#define SCREENSHOT_H
    #include <SFML/Graphics.h>
    #include <stdbool.h>

    #define SCREENSHOT_DIR "screenshots"
    #define MAX_SCREENSHOT_FRAMES 64u

// Frames to capture automatically, read from the AQUASSIM_SCREENSHOTS environment variable
typedef struct screenshot_plan {
    int frames[MAX_SCREENSHOT_FRAMES];
    unsigned int count;
    unsigned int next;
} screenshot_plan_t;

bool screenshot_save(sfRenderWindow *window, int frame);
void screenshot_plan_load(screenshot_plan_t *plan);
bool screenshot_plan_should_capture(screenshot_plan_t *plan, int frame);
bool screenshot_plan_done(const screenshot_plan_t *plan);

#endif // SCREENSHOT_H
