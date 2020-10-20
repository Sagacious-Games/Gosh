#include <stdlib.h>
#include <stdio.h>

#include "backend.h"
#include "window.h"
#include "../../window.h"
#include "../../event.h"

backend_x11_t *create_backend_x11 () {

    backend_x11_t *backend = malloc (sizeof (backend_x11_t));
    backend->n_windows = 0;

    if (!(backend->display = XOpenDisplay (NULL))) {

        fputs ("Failed to open X display.\n", stderr);
        exit (EXIT_FAILURE);
    }

    return backend;
}

void destroy_backend_x11 (backend_x11_t *backend) {

    XCloseDisplay (backend->display);
    free (backend);
}

void register_window_x11 (backend_x11_t *backend, window_x11_t *window) {

    backend->windows[backend->n_windows++] = window;

    /* TODO: handle this more elegantly lol */
    if (backend->n_windows == MAX_WINDOWS)
        fputs ("Warning: opened too many windows!", stderr);
}

window_x11_t *lookup_window_x11 (backend_x11_t *backend, Window window) {

    int i;
    for (i = 0; i < backend->n_windows; i++)
        if (backend->windows[i]->window_handle == window)
            return backend->windows[i];

    return NULL;
}

void backend_x11_run (backend_x11_t *backend) {

    event_t window_event;
    XEvent x11_event;
    window_x11_t *window;

    for (;;) {
        
        XNextEvent (backend->display, &x11_event);
        
        switch (x11_event.type) {

            case MapNotify: {

                XMapEvent event = x11_event.xmap;
                window = lookup_window_x11 (backend, event.window);
                window_event.type = EVENT_MAP;

                break;
            }

            case UnmapNotify: {

                XUnmapEvent event = x11_event.xunmap;
                window = lookup_window_x11 (backend, event.window);
                window_event.type = EVENT_UNMAP;

                break;
            }

            case Expose: {

                XExposeEvent event = x11_event.xexpose;
                window = lookup_window_x11 (backend, event.window);

                window_event.type                              = EVENT_EXPOSE;
                window_event.events.expose_region.offset.x     = event.x;
                window_event.events.expose_region.offset.y     = event.y;
                window_event.events.expose_region.dimensions.x = event.width;
                window_event.events.expose_region.dimensions.y = event.height;

                break;
            }

            case ButtonPress: {

                XButtonPressedEvent event = x11_event.xbutton;
                window = lookup_window_x11 (backend, event.window);

                window_event.type                      = EVENT_MOUSE;
                window_event.events.mouse.type         = EVENT_MOUSE_BUTTON;
                window_event.events.mouse.position.x   = event.x;
                window_event.events.mouse.position.y   = event.y;
                window_event.events.mouse.button       = MOUSE_LEFT; /* TODO */
                window_event.events.mouse.button_state = MOUSE_BUTTON_PRESSED;

                break;
            }

            case ButtonRelease: {

                XButtonReleasedEvent event = x11_event.xbutton;
                window = lookup_window_x11 (backend, event.window);

                window_event.type                      = EVENT_MOUSE;
                window_event.events.mouse.type         = EVENT_MOUSE_BUTTON;
                window_event.events.mouse.position.x   = event.x;
                window_event.events.mouse.position.y   = event.y;
                window_event.events.mouse.button       = MOUSE_LEFT; /* TODO */
                window_event.events.mouse.button_state = MOUSE_BUTTON_RELEASED;

                break;
            }

            /* TODO: mouse motion */

            case KeyPress: {

                XKeyPressedEvent event = x11_event.xkey;
                window = lookup_window_x11 (backend, event.window);

                window_event.type                      = EVENT_KEYBOARD;
                window_event.events.keyboard.key       = KEYBOARD_KEY_Q; /* TODO */
                window_event.events.keyboard.key_state = KEYBOARD_KEY_PRESSED;

                break;
            }

            case KeyRelease: {

                XKeyReleasedEvent event = x11_event.xkey;
                window = lookup_window_x11 (backend, event.window);

                window_event.type                      = EVENT_KEYBOARD;
                window_event.events.keyboard.key       = KEYBOARD_KEY_Q; /* TODO */
                window_event.events.keyboard.key_state = KEYBOARD_KEY_RELEASED;

                break;
            }

            /* TODO: text input */

            default:
                continue;
        }

        handle_event (window->window, window_event);
    }
}
