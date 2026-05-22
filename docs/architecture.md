# Architecture

This document describes how Lumine is structured and the reasoning behind the
key decisions. It is updated every iteration.

## Goals

Lumine is optimised for four properties, in priority order:

1. **Responsiveness** — the UI thread must never block.
2. **Low memory use** — avoid unnecessary copies and decoded-image retention.
3. **Fast startup** — minimal work before the first frame is shown.
4. **Simplicity** — small, single-responsibility modules.

## Module overview

Each module is a directory under `src/`. Dependencies only ever point
*downward* in this table — UI depends on rendering and image code, never the
other way around.

| Module      | Responsibility                                   | Depends on        |
| ----------- | ------------------------------------------------ | ----------------- |
| `app`       | Process entry point, CLI parsing, app bootstrap  | `ui`, `core`, `platform` |
| `ui`        | Windows, widgets, keyboard input, status display | `rendering`, `image`, `core` |
| `rendering` | Drawing an image to the screen (the canvas)      | —                 |
| `image`     | Decoding image files into in-memory bitmaps      | `core`            |
| `platform`  | Probing the Wayland / desktop environment        | —                 |
| `core`      | Cross-cutting utilities (logging, version)       | —                 |

This direction is enforced socially for now; a future iteration may split the
modules into separate CMake libraries to enforce it mechanically.

### Why a separate `lumine_lib`

All logic is compiled into a static library, `lumine_lib`. The executable
(`lumine`) is a thin shell that only contains `main()`. This lets the test
suite link and exercise the *exact same* object code, with no recompilation
and no risk of test/production divergence.

## Separation of rendering and UI

`rendering::ImageView` knows nothing about files, navigation, or application
state. It is handed a `QImage` and draws it — nothing more. All policy
(what to open, when, what to show on failure) lives in `ui::MainWindow`.

This boundary keeps the rendering path swappable. Iteration 1 uses Qt's
Graphics View framework; if a later phase needs a hand-written OpenGL or
QRhi-based canvas, only `ImageView` changes.

## Rendering pipeline

```
file path ──▶ ImageLoader::load ──▶ QImage ──▶ ImageView::setImage
                                                    │
                                       QPixmap::fromImage (CPU→GPU upload)
                                                    │
                                   QGraphicsScene / QGraphicsPixmapItem
                                                    │
                                   QGraphicsView::fitInView (scale transform)
```

- **`QGraphicsView`** was chosen over a raw `paintEvent` widget because it
  provides scene transforms (scale, translate) for free. Phase 2's zoom and
  pan become transform tweaks rather than manual geometry math.
- The viewport can later be swapped for a `QOpenGLWidget` to route all
  compositing through the GPU, without touching application code.
- `Qt::SmoothTransformation` and `SmoothPixmapTransform` give bilinear
  scaling so down-scaled images stay clean.

### Fit-to-window behaviour

`ImageView` always fits the image to the viewport, preserving aspect ratio,
and re-fits on every `resizeEvent` and `showEvent`. Iteration 1 has no zoom,
so this is the entire interaction model. Phase 2 introduces a real zoom state
machine (fit / 100% / free zoom) layered on top.

## Threading model

Decoding runs off the UI thread. `ImageLoader::load` itself stays
synchronous and stateless; `AsyncImageLoader` wraps it for concurrency:

- `AsyncImageLoader::request` returns immediately. The UI thread never
  blocks on I/O or decoding.
- A small `QThreadPool` (two threads) runs `ImageLoader::load` on a worker.
- The worker hands its `QImage` back through a queued `QMetaObject::invokeMethod`
  call. `QImage` is implicitly shared, so the handoff copies no pixels.
- Each request carries a monotonic **generation token**. When a newer
  request arrives the token advances, and the delivery slot drops any result
  whose token is stale — so rapid navigation never flashes a superseded
  image. This is cooperative cancellation: a decode already running is not
  interrupted, only its result is discarded.

Everything that touches widgets stays on the UI thread; only pure decoding
crosses the boundary.

## Cache strategy

`core::ImageCache` is a least-recently-used cache of decoded `QImage`s,
keyed by absolute path and bounded by a **byte budget** (256 MB by default).
Inserting an entry past the budget evicts the least-recently-used entries
until it fits; an image larger than the whole budget is never stored.
`get()` refreshes recency. It is pure logic — no threading, no I/O — and is
driven only from the UI thread.

**Preloading.** Whenever the current image changes, `MainWindow` asks
`AsyncImageLoader` to speculatively decode the next and previous images via
low-priority `requestPreload` calls. Their results land in the cache, so
stepping to a neighbour is usually an instant cache hit with no decode and
no flicker. Opening a new directory clears the cache, since the previous
neighbourhood will not be revisited.

**Thumbnails** follow the same async pattern: `ThumbnailLoader` decodes
down-scaled previews on a thread pool (using `QImageReader::setScaledSize`
to avoid a full-resolution decode), and `ThumbnailStrip` fills them in as
they arrive.

## Memory and performance notes

- `LoadResult` carries a `QImage` by value; `QImage` is copy-on-write, so
  returning it copies a handle, not pixels.
- `reader.read()` writes directly into the result image — no intermediate.
- No owning raw pointers: Qt objects are held in the parent-child tree
  (a form of RAII — the parent destroys its children), and value types use
  automatic storage.

## Wayland considerations

- The Wayland platform plugin is auto-selected by Qt; Lumine sets no
  X11-specific hints.
- `QGuiApplication::setDesktopFileName("io.github.lumine.Lumine")` ties the
  Wayland surface to the installed `.desktop` file. On Wayland there is **no
  client-side window-icon protocol** — the compositor (e.g. Hyprland) resolves
  the icon and app identity from that desktop file via the surface `app_id`.
  This is why the desktop file's basename, the `StartupWMClass`, and the value
  passed to `setDesktopFileName` must all agree.
- `HighDpiScaleFactorRoundingPolicy::PassThrough` preserves fractional
  display scaling instead of rounding it — important on Hyprland multi-monitor
  setups with non-integer scales.

## Decision log

| Decision                              | Rationale                                    |
| ------------------------------------- | -------------------------------------------- |
| Qt 6 Widgets + Graphics View          | Mature, fast startup, free scene transforms  |
| Static `lumine_lib` + thin executable | Tests link identical code                    |
| Catch2 v3                             | Lightweight, expressive, easy CMake wiring   |
| Synchronous loading in Iteration 1    | Simplest correct thing; async lands Phase 2  |
| Headers beside sources                | Simpler than a split `include/` for an app   |
