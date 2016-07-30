#include <stdio.h>
#include <sys/types.h>
#include <sys/poll.h>

#include <libinput.h>

#include "core.h"
#include "config.h"
#include "macros.h"

/* Notes to self:
 *
 * * To get the keyboard names, use libevdev_event_code_get_name() - see
 *   event-debug.c:209 in libinput sources.
 *
*/

int main(void)
{
    /* Declarations */
    struct libinput* li;
    struct pollfd fds[1];

    struct libinput_event* ev = NULL;
    struct beaugeste_config cfg;

    /* Initialisation */
    li = beaugeste_init();
    char* path = beaugeste_get_config_path();
    beaugeste_open_config_file(&cfg, path);
    free(path);

    /* Set up the event polling. */
    fds[0].fd = libinput_get_fd(li);
    fds[0].events = POLLIN;

    /* Main bit */
    while (1) {
        // Wait forever until we get something to do.
        poll(fds, 1, -1);
        libinput_dispatch(li); // Then, dispatch events!

        // Pull the next event off the pile.
        ev = libinput_get_event(li);
        if (ev == NULL){
            // No event for us :(
            continue;
        }

        // Handle the event.
        beaugeste_handle_event(&cfg, ev);

        // Free memory.
        libinput_event_destroy(ev);
    }

    /* Wrap things up. */
    beaugeste_close_config_file(&cfg);
    beaugeste_exit(li);

    return 0;
}
