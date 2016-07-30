#include <sys/types.h>
#include <string.h>

#include "core.h"
#include "macros.h"

static const char* BEAUGESTE_DIRECTION_NAMES[] = {
    "east",
    "north-east",
    "north",
    "north-west",
    "west",
    "south-west",
    "south",
    "south-east",
    NULL,
};

// Interface calls.
static int open_restricted(const char* path, int flags, void* d){
    UNUSED(d);
    int fd = open(path, flags);
    if (fd < 0){ return -1; }
    else { return fd; }
}

static void close_restricted(int fd, void* d){
    UNUSED(d);
    close(fd);
}

static const struct libinput_interface IFACE = {
    open_restricted,
    close_restricted,
};


double beaugeste_movement_get_direction(double dx, double dy){
    return atan2(dy, dx);
}

enum beaugeste_direction beaugeste_movement_get_cardinal(double dx, double dy){
    double absx = abs(dx);
    double absy = abs(dy);

    if (absx > absy){
        if (dx >= 0){
            return BEAUGESTE_EAST;
        }
        else {
            return BEAUGESTE_WEST;
        }
    }
    else {
        if (dy >= 0){
            return BEAUGESTE_SOUTH;
        }
        else {
            return BEAUGESTE_NORTH;
        }
    }
}

struct libinput* beaugeste_init(){
    struct libinput* li;
    struct udev* udev;

    udev = udev_new();
    if (NULL == udev){
        // Something has gone wrong.
        // Are we sure udev is even running?
        ERR("Couldn't set up udev.");
        return NULL;
    }

    li = libinput_udev_create_context(&IFACE, NULL, udev);
    if (NULL == li){
        ERR("Failed to create the libinput context.");
        udev_unref(udev);
        return NULL;
    }

    // Assign a seat.
    // In the future, we will want to do this by some means other than hardcoding...
    libinput_udev_assign_seat(li, "seat0");

    // That is all, udev.
    udev_unref(udev);

    return li;
}

void beaugeste_exit(struct libinput *li){
    COND_UNREF(li, libinput_unref);
}

void beaugeste_handle_event(struct beaugeste_config *cfg,
                            struct libinput_event* ev){
    static struct beaugeste_swipe sw;
    enum libinput_event_type kind = libinput_event_get_type(ev);

    switch (kind){
    case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
        beaugeste_make_swipe(ev, &sw);
        break;

    case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
        beaugeste_update_swipe(ev, &sw);
        break;

    case LIBINPUT_EVENT_GESTURE_SWIPE_END:
        beaugeste_handle_swipe(ev, &sw, cfg);
        break;

    default: break;
    }
}

void beaugeste_make_swipe(struct libinput_event* ev, struct beaugeste_swipe* sw){
    assert(libinput_event_get_type(ev) == LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN);
    assert(sw != NULL);

    // Zero the whole structure, so we know what we're starting with.
    memset(sw, 0, sizeof(struct beaugeste_swipe));

    struct libinput_event_gesture* geste = libinput_event_get_gesture_event(ev);

    sw->direction = BEAUGESTE_NO_DIRECTION;
    sw->_list_pos = 0;
    sw->finger_count = libinput_event_gesture_get_finger_count(geste);
}

static double _calculate_total_recent_distance(double nums[], uint32_t times[],
                                               int count, uint32_t now){
    int ii = 0;
    double total = 0.0;
    uint32_t this_time = 0;

    for (ii = 0; ii < count; ii++){
        this_time = times[ii];
        if (now - this_time > 150){
            // Too old; does not want.
            continue;
        }
        total += nums[ii];
    }
    return total;
}

void beaugeste_update_swipe(struct libinput_event* ev, struct beaugeste_swipe* sw){
    assert(libinput_event_get_type(ev) == LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE);
    assert(sw != NULL);

    struct libinput_event_gesture* geste = libinput_event_get_gesture_event(ev);

    double dx = libinput_event_gesture_get_dx(geste);
    double dy = libinput_event_gesture_get_dy(geste);
    uint32_t now = libinput_event_gesture_get_time(geste);

    uint32_t diff;

    // Update the recent delta list.
    sw->recent_delta_x[sw->_list_pos] = dx;
    sw->recent_delta_y[sw->_list_pos] = dy;
    sw->update_timestamps[sw->_list_pos] = now;
    sw->_list_pos ++;
    sw->_list_pos %= 20;

    // And average that into a movement direction.
    double total_dx = 0, total_dy = 0;
    total_dx = _calculate_total_recent_distance(sw->recent_delta_x,
                                                sw->update_timestamps,
                                                20, now);
    total_dy = _calculate_total_recent_distance(sw->recent_delta_y,
                                                sw->update_timestamps,
                                                20, now);

    sw->direction = beaugeste_movement_get_cardinal(total_dx, total_dy);
}

void beaugeste_handle_swipe(struct libinput_event* ev, struct beaugeste_swipe *sw,
                            struct beaugeste_config* cfg){
    struct libinput_event_gesture *geste = libinput_event_get_gesture_event(ev);

    // Obviously, if the gesture was cancelled, don't do anything.
    if (libinput_event_gesture_get_cancelled(geste)
            || sw->direction >= BEAUGESTE_NUM_DIRECTIONS){
        return;
    }

    // Also if it was too short, it's not meaningful.
    double total_dx = 0, total_dy = 0;
    uint32_t now = libinput_event_gesture_get_time(geste);
    total_dx = _calculate_total_recent_distance(sw->recent_delta_x,
                                                sw->update_timestamps,
                                                20, now);
    total_dy = _calculate_total_recent_distance(sw->recent_delta_y,
                                                sw->update_timestamps,
                                                20, now);
    if (fabs(total_dx) < 50 && fabs(total_dy) < 50){
        return;
    }

    // Do something.
    char* key = NULL;
    const char* name = BEAUGESTE_DIRECTION_NAMES[sw->direction];
    asprintf(&key, "swipe-%d-%s", sw->finger_count, name);
    beaugeste_config_act(cfg, key);
//    char* operation = beaugeste_config_read_string(cfg, key, NULL);
//    if (operation != NULL){
//        system(operation);
//    }

    free(key);
}
