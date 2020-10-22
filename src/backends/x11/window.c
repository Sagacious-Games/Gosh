#include <stdlib.h>
#include <stdio.h>

#include <X11/Xutil.h>

#include <gosh/backends/x11/window.h>
#include <gosh/window.h>

window_x11_t *create_window_x11 (backend_x11_t *backend,
                                 window_t *window,
                                 region_t region,
                                 char *title) {

    XSetWindowAttributes attributes;

    XSizeHints normal_hints;
    XWMHints hints;
    XClassHint class_hints;

    XTextProperty window_name, icon_name;

    window_x11_t *window_x11 = malloc (sizeof (window_x11_t));
    window_x11->backend = backend;
    window_x11->window = window;
    window_x11->image = NULL;

    window_x11->window_handle = XCreateWindow (backend->display,
                                               DefaultRootWindow (backend->display),
                                               region.offset.x,
                                               region.offset.y, 
                                               region.dimensions.x,
                                               region.dimensions.y,
                                               0,
                                               CopyFromParent,
                                               CopyFromParent,
                                               CopyFromParent,
                                               0,
                                               &attributes);

    /* configure the window title name */
    if (XStringListToTextProperty (&title, 1, &window_name) == 0) {

        fputs ("Failed to allocate XTextProperty for window name.", stderr);
	    exit (EXIT_FAILURE);
    }

    /* configure the icon name (name the window is iconized) */
    if (XStringListToTextProperty (&title, 1, &icon_name) == 0) {

        fputs ("Failed to allocate XTextProperty for icon name.", stderr);
	    exit (EXIT_FAILURE);
    }

    /* configure normal (size) hints */
    normal_hints.flags = 0;

    /* configure window manager hints */
    hints.flags = StateHint;
    hints.initial_state = NormalState;

    /* configure class hints */
    class_hints.res_name = title;
    class_hints.res_class = title;

    XSetWMProperties (backend->display,
                      window_x11->window_handle,
                      &window_name,
                      &icon_name,
                      NULL, 0,
                      &normal_hints,
                      &hints,
                      &class_hints);

    XSelectInput (backend->display, window_x11->window_handle,
                  ExposureMask        | ButtonPressMask | ButtonReleaseMask |
                  PointerMotionMask   | KeyPressMask    | KeyReleaseMask    |
                  StructureNotifyMask | EnterWindowMask | LeaveWindowMask);

    XMapRaised (backend->display, window_x11->window_handle);

    /* register the newly created window with the backend */
    register_window_x11 (backend, window_x11);

    return window_x11;
}

void destroy_window_x11_buffer (window_x11_t *window) {

    XDestroyImage (window->image);
}

void destroy_window_x11 (window_x11_t *window) {

    destroy_window_x11_buffer (window);
    XDestroyWindow (window->backend->display, window->window_handle);
    free (window);
}

void update_window_x11_region (window_x11_t *window, region_t region) {

    Display *display = window->backend->display;
    GC graphics_context = DefaultGC (display, DefaultScreen (display));
    XPutImage (display,
               window->window_handle,
               graphics_context,
               window->image,
               region.offset.x,
               region.offset.y,
               region.offset.x,
               region.offset.y,
               region.dimensions.x,
               region.dimensions.y);
}

void init_window_x11_buffer (window_x11_t *window, region_t region) {

    /* TODO: figure out bits per pixel and stuff??????? */

    Display *display = window->backend->display;
    int depth = DefaultDepth (display, DefaultScreen (display));

    /* free the existing image if it exists */
    if (window->image)
        destroy_window_x11_buffer (window);

    /* configure the buffer spec for the new image */
    window->window->buffer.region = region;
    window->window->buffer.pixels
        = malloc (region.dimensions.x * region.dimensions.y * 4);

    /* create the new image matching the window's dimensions */
    window->image = XCreateImage (window->backend->display,
                                  CopyFromParent,
                                  depth,
                                  ZPixmap,
                                  0,
                                  window->window->buffer.pixels,
                                  region.dimensions.x,
                                  region.dimensions.y,
                                  32,
                                  0);
}
