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
- [x] Arrow-key navigation across a folder of images

## Phase 2 — Interaction & async

- [x] Mouse-wheel zoom (fit / 100% / free zoom states)
- [x] Drag-to-pan
- [x] Image-list model built from the opened file's directory
- [x] Asynchronous, cancellable decoding off the UI thread
- [x] Smooth render updates during rapid navigation

## Phase 3 — Caching & preloading

- [x] LRU decoded-image cache with a byte budget
- [x] Preload the next and previous images
- [x] Thumbnail sidebar
- [x] Cache eviction tests

## Phase 4 — Experience

- [x] Subtle fade transition on image change
- [x] Image properties panel (file & pixel metadata)
- [x] Slideshow mode
- [x] Recent files

> Camera EXIF tag extraction (exposure, ISO, lens) is a future addition —
> it needs a dedicated parser or a library such as exiv2. The current
> properties panel covers file facts and decoded-pixel facts.

## Phase 5 — Performance & packaging

- [x] GPU-backed OpenGL viewport
- [x] Bounded decode that caps very large images
- [x] Performance profiling (scoped timers, startup timing)
- [x] PKGBUILD and CPack packaging

## Iteration history

| Iteration | Summary                                              |
| --------- | ---------------------------------------------------- |
| 1         | Project bootstrap, window, basic image display       |
| 2         | Zoom/pan, folder navigation, async decoding           |
| 3         | LRU cache, neighbour preloading, thumbnail strip      |
| 4         | Fade transitions, info panel, slideshow, recent files |
| 5         | GPU viewport, huge-image cap, profiling, packaging    |

## Future directions

Beyond the five-phase plan:

- Tiled / progressive rendering for true gigapixel images
- Camera EXIF tag extraction (exposure, ISO, lens) via exiv2
- Basic edits — rotate, crop — with non-destructive save
- A settings dialog for cache budget, slideshow interval and theme
