#include "screenshot.h"
#include "aquassim.h"
#include <sys/stat.h>
#include <errno.h>

// Save the current content of the window to screenshots/frame_XXXXXX.png
// Must be called after drawing and before sfRenderWindow_display
bool screenshot_save(sfRenderWindow *window, int frame) {
    sfVector2u size = sfRenderWindow_getSize(window);
    sfTexture *texture = NULL;
    sfImage *image = NULL;
    char path[256] = {0};
    bool saved = false;

    if (mkdir(SCREENSHOT_DIR, 0755) != 0 && errno != EEXIST) {
        LOG("[ERROR] Failed to create the %s directory\n", SCREENSHOT_DIR);
        return false;
    }
    texture = sfTexture_create(size.x, size.y);
    if (!texture) {
        LOG("[ERROR] Failed to create the screenshot texture\n");
        return false;
    }
    sfTexture_updateFromRenderWindow(texture, window, 0, 0);
    image = sfTexture_copyToImage(texture);
    snprintf(path, sizeof(path), "%s/frame_%06d.png", SCREENSHOT_DIR, frame);
    if (image && sfImage_saveToFile(image, path)) {
        LOG("[INFO] Screenshot saved to %s\n", path);
        saved = true;
    } else {
        LOG("[ERROR] Failed to save the screenshot to %s\n", path);
    }
    if (image) {
        sfImage_destroy(image);
    }
    sfTexture_destroy(texture);
    return saved;
}

// Read a comma separated list of frames, e.g. AQUASSIM_SCREENSHOTS=60,300,600
void screenshot_plan_load(screenshot_plan_t *plan) {
    const char *env = getenv("AQUASSIM_SCREENSHOTS");
    char *end = NULL;

    plan->count = 0;
    plan->next = 0;
    while (env && *env && plan->count < MAX_SCREENSHOT_FRAMES) {
        long frame = strtol(env, &end, 10);
        if (end == env) {
            LOG("[ERROR] Invalid AQUASSIM_SCREENSHOTS value near \"%s\"\n", env);
            break;
        }
        plan->frames[plan->count++] = (int)frame;
        env = (*end == ',') ? end + 1 : end;
    }
}

bool screenshot_plan_should_capture(screenshot_plan_t *plan, int frame) {
    // Frames are expected in increasing order; skip any that were already passed
    while (plan->next < plan->count && plan->frames[plan->next] < frame) {
        plan->next++;
    }
    if (plan->next < plan->count && plan->frames[plan->next] == frame) {
        plan->next++;
        return true;
    }
    return false;
}

bool screenshot_plan_done(const screenshot_plan_t *plan) {
    return plan->count > 0 && plan->next >= plan->count;
}
