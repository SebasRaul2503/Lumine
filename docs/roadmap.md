# Roadmap

Lumine is built in small, always-shippable iterations. Each iteration ends
with compiling, runnable code and a clean git checkpoint.

Legend: `[x]` done · `[~]` in progress · `[ ]` planned

## Phase 1 — Foundations

- [x] Project bootstrap: CMake, directory structure, tooling
- [x] Qt 6 application and main window
- [x] Basic synchronous image loading with error reporting
- [x] Open an image from a CLI argument
- [x] Centred display with resize-aware fit-to-window scaling
- [x] Fullscreen support
- [x] Wayland-friendly configuration (HiDPI, desktop identity)
- [x] clang-format, clang-tidy, initial docs and tests
- [ ] Arrow-key navigation across a folder of images

## Phase 2 — Interaction & async

- [ ] Mouse-wheel zoom (fit / 100% / free zoom states)
- [ ] Drag-to-pan
- [ ] Image-list model built from the opened file's directory
- [ ] Asynchronous, cancellable decoding off the UI thread
- [ ] Smooth render updates during rapid navigation

## Phase 3 — Caching & preloading

- [ ] LRU decoded-image cache with a byte budget
- [ ] Preload the next and previous images
- [ ] Thumbnail sidebar
- [ ] Cache eviction tests

## Phase 4 — Experience

- [ ] Subtle open / navigation transitions
- [ ] EXIF metadata panel
- [ ] Slideshow mode
- [ ] Recent files

## Phase 5 — Performance & packaging

- [ ] GPU texture reuse and an OpenGL/QRhi viewport
- [ ] Progressive loading for very large images
- [ ] Performance profiling and startup-time budget
- [ ] PKGBUILD and AUR-ready packaging

## Iteration history

| Iteration | Summary                                              |
| --------- | ---------------------------------------------------- |
| 1         | Project bootstrap, window, basic image display       |
