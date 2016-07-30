#ifndef CORE_H
#define CORE_H

#include <math.h>

#include <libinput.h>
#include <libudev.h>

#include "config.h"

enum beaugeste_direction {
    BEAUGESTE_EAST = 0,
    BEAUGESTE_NORTH_EAST,
    BEAUGESTE_NORTH,
    BEAUGESTE_NORTH_WEST,
    BEAUGESTE_WEST,
    BEAUGESTE_SOUTH_WEST,
    BEAUGESTE_SOUTH,
    BEAUGESTE_SOUTH_EAST,

    BEAUGESTE_NUM_DIRECTIONS,
    BEAUGESTE_NO_DIRECTION = 99,
};

enum beaugeste_gesture_type {
    BEAUGESTE_NO_GESTURE = 0,

    BEAUGESTE_SWIPE = 1,
    BEAUGESTE_PINCH,
    BEAUGESTE_TOUCH,
    BEAUGESTE_POINTER,
    BEAUGESTE_KEY,

    BEAUGESTE_DEVICE = 256,
};

// Populated while we receive GESTURE_SWIPE_{BEGIN,UPDATE,END} events.
// Then, this is handed off to be processed (i.e. possibly trigger some action)
struct beaugeste_swipe {
    // Consider only the last 20 events. Also, ignore events that are too old.
    double recent_delta_x[20];
    double recent_delta_y[20];
    uint32_t update_timestamps[20];
    uint32_t _list_pos; // Where we are in the list.

    enum beaugeste_direction direction;

    uint32_t finger_count;
};

struct beaugeste_state {
    // What are we currently processing.
    enum beaugeste_gesture_type current_gesture;

    // The direction of a current swipe, if it exists.

    enum beaugeste_direction swipe_direction;

    // How many fingers an active swipe has.
    uint32_t finger_count;
};

// Determine which direction a "thing" is moving in.
// The first returns an angle in radians; the second, a cardinal direction.
double beaugeste_movement_get_direction(double dx, double dy);
enum beaugeste_direction beaugeste_movement_get_cardinal(double dx, double dy);

// Start up/shut down Beau Geste.
struct libinput* beaugeste_init();
void beaugeste_exit(struct libinput* li);

// What do we do with an event?
void beaugeste_handle_event(struct beaugeste_config* cfg, struct libinput_event* ev);

void beaugeste_update_swipe(struct libinput_event* ev, struct beaugeste_swipe* sw);
void beaugeste_make_swipe(struct libinput_event* ev, struct beaugeste_swipe* sw);
void beaugeste_handle_swipe(struct libinput_event* ev, struct beaugeste_swipe* sw,
                            struct beaugeste_config* cfg);

#endif // CORE_H
