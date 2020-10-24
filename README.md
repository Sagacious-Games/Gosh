# Gosh

Simple and easy to use cross-platform window system API.

It aims to be similar to SDL, but more lightweight, and designed around giving you easy access to the window's buffer in software.

## Try the demo

Just run this command in the root of the repository!:

```bash
make debug
```

## Build and install

If you just built the debug version, be sure to clean first:

```bash
make clean
```

(Otherwise you won't get optimizations!)

When you are ready to build and install:

```bash
make install
```

## TODO

### General

- Setup for multiple demos showcasing different features
- Keeping proper track of closed windows
- Mechanism for detecting when last window is closed

### X11

- Complete the keycode mappings
- Proper generic color handling in X11
- Some kind of X11 vsync mechanism and associated event
- Filter out repeat resize events in event queue (to avoid resize drawing lag)
- X11 MIT-SHM extension when available
- Fix "BadDrawable" on attempt to draw while window is closing

### Backends

- Auto detect backend (with environment variable override)
- Windows backend
- Wayland backend
- MacOS backend
