# Lumine

A fast, modern, GPU-accelerated image viewer for Linux Wayland compositors,
built with **C++20** and **Qt 6**. Lumine targets Hyprland and other Wayland
desktops, with an emphasis on instant startup, low memory use, and a quiet,
keyboard-driven interface.

> **Status — early development.** Iteration 1 (project bootstrap) is complete.
> See [`docs/roadmap.md`](docs/roadmap.md) for what is planned next.

## Features

Available now:

- Native Wayland window with HiDPI / fractional-scaling support
- Open an image from the command line or a file dialog
- Wheel zoom (fit / 100% / free), cursor-anchored, with drag-to-pan
- Folder navigation — arrow keys step through the opened file's directory
- Asynchronous decoding off the UI thread, with stale-result cancellation
- LRU decoded-image cache with speculative next/previous preloading
- Thumbnail sidebar for click-to-jump navigation
- Fullscreen mode and a keyboard-first control scheme
- Dark, minimal interface

Planned: open/navigation transitions, EXIF metadata, slideshow mode,
recent files, and a GPU-backed viewport.

## Quick start

```sh
# 1. Install dependencies (Arch Linux)
sudo pacman -S --needed base-devel cmake qt6-base qt6-wayland catch2 clang

# 2. Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# 3. Run
./build/src/lumine path/to/image.png
```

Full instructions are in [`docs/build.md`](docs/build.md).

## Keyboard shortcuts

| Shortcut          | Action                          |
| ----------------- | ------------------------------- |
| `Ctrl+O`          | Open an image                   |
| `←` / `→` / `Space` | Previous / next image         |
| `+` / `-` / wheel | Zoom in / out                   |
| `0` / `1`         | Zoom to fit / actual size       |
| Double-click      | Toggle fit and 1:1              |
| `T`               | Toggle the thumbnail strip      |
| `F` / `F11`       | Toggle fullscreen               |
| `Esc`             | Exit fullscreen, or quit        |
| `Ctrl+Q`          | Quit                            |

## Documentation

- [Architecture](docs/architecture.md) — modules, threading, rendering pipeline
- [Build guide](docs/build.md) — dependencies and build options
- [Testing](docs/testing.md) — running and writing tests
- [Roadmap](docs/roadmap.md) — phased feature plan

## Project layout

```
src/
  app/         Executable entry point
  core/        Cross-cutting utilities (logging, version)
  image/       Image decoding
  rendering/   GPU-friendly image canvas
  ui/          Windows, widgets, input handling
  platform/    Wayland / desktop environment probing
tests/         Catch2 test suite
docs/          Architecture and developer documentation
cmake/         Reusable CMake modules
assets/        Desktop entry and icons
scripts/       Developer tooling
```

## License

Lumine is released under the MIT License. See [`LICENSE`](LICENSE).
