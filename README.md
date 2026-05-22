# Lumine

A fast, modern, GPU-accelerated image viewer for Linux Wayland compositors,
built with **C++20** and **Qt 6**. Lumine targets Hyprland and other Wayland
desktops, with an emphasis on instant startup, low memory use, and a quiet,
keyboard-driven interface.

> **Status — v0.1.0.** All five planned development phases are complete.
> Lumine builds cleanly (C++20, warnings-as-errors) and runs natively on
> Wayland / Hyprland with a GPU-backed viewport. The test suite has 45 cases
> (165 assertions). See [`docs/roadmap.md`](docs/roadmap.md) for what's next.

## Features

Available now:

- Native Wayland window with HiDPI / fractional-scaling support
- Open an image from the command line or a file dialog
- Wheel zoom (fit / 100% / free), cursor-anchored, with drag-to-pan
- Folder navigation — arrow keys step through the opened file's directory
- Asynchronous decoding off the UI thread, with stale-result cancellation
- LRU decoded-image cache with speculative next/previous preloading
- Thumbnail sidebar for click-to-jump navigation
- Subtle fade transition when the image changes
- Image properties panel and a slideshow mode
- Recent files, reachable from a quick-pick menu
- GPU-backed OpenGL viewport; very large images decode bounded
- Fullscreen mode and a keyboard-first control scheme
- Dark, minimal interface

See [`docs/roadmap.md`](docs/roadmap.md) for future directions — tiled
rendering for gigapixel images, EXIF tags, and basic edits.

## Quick start

```sh
# 1. Install dependencies (Arch Linux)
sudo pacman -S --needed base-devel cmake qt6-base qt6-wayland catch2 clang

# 2. Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# 3. Run from the build tree …
./build/src/lumine path/to/image.png

# … or install it system-wide (also registers it with your app launcher)
sudo cmake --install build
```

> Running the **uninstalled** binary logs a harmless `org.freedesktop.portal`
> warning about an unknown app ID — installing the desktop entry (the last
> step above) resolves it. It does not affect functionality.

Full instructions are in [`docs/build.md`](docs/build.md).

## Keyboard shortcuts

| Shortcut          | Action                          |
| ----------------- | ------------------------------- |
| `Ctrl+O`          | Open an image                   |
| `Ctrl+R`          | Recent files menu               |
| `←` / `→` / `Space` | Previous / next image         |
| `+` / `-` / wheel | Zoom in / out                   |
| `0` / `1`         | Zoom to fit / actual size       |
| Double-click      | Toggle fit and 1:1              |
| `T`               | Toggle the thumbnail strip      |
| `I`               | Toggle the properties panel     |
| `S`               | Start / stop the slideshow      |
| `F` / `F11`       | Toggle fullscreen               |
| `Esc`             | Exit fullscreen, or quit        |
| `Ctrl+Q`          | Quit                            |

## Documentation

- [Architecture](docs/architecture.md) — modules, threading, rendering pipeline
- [Build guide](docs/build.md) — dependencies and build options
- [Testing](docs/testing.md) — running and writing tests
- [Packaging](docs/packaging.md) — install, PKGBUILD, CPack tarballs
- [Roadmap](docs/roadmap.md) — phased feature plan and history

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
packaging/     Arch Linux PKGBUILD
scripts/       Developer tooling
```

## License

Lumine is released under the MIT License. See [`LICENSE`](LICENSE).
