# Lumine

A fast, modern, GPU-accelerated image viewer for Linux Wayland compositors,
built with **C++20** and **Qt 6**. Lumine targets Hyprland and other Wayland
desktops, with an emphasis on instant startup, low memory use, and a quiet,
keyboard-driven interface.

> **Status — early development.** Iteration 1 (project bootstrap) is complete.
> See [`docs/roadmap.md`](docs/roadmap.md) for what is planned next.

## Features

Available now (Iteration 1):

- Native Wayland window with HiDPI / fractional-scaling support
- Open an image from the command line or a file dialog
- Fit-to-window display that re-scales smoothly on resize
- Fullscreen mode and a keyboard-first control scheme
- Dark, minimal interface

Planned: interactive zoom & pan, folder navigation, asynchronous preloading,
a thumbnail sidebar, EXIF metadata, slideshow mode, and GPU texture caching.

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

| Shortcut    | Action                          |
| ----------- | ------------------------------- |
| `Ctrl+O`    | Open an image                   |
| `F` / `F11` | Toggle fullscreen               |
| `Esc`       | Exit fullscreen, or quit        |
| `Ctrl+Q`    | Quit                            |

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
