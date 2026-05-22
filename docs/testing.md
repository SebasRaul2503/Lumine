# Testing

Lumine uses [Catch2 v3](https://github.com/catchorg/Catch2) for unit tests.
Every iteration adds tests for the logic it introduces, including failure
paths and edge cases.

## Running the tests

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

Or run the test binary directly for Catch2's richer output:

```sh
./build/tests/lumine_tests            # run everything
./build/tests/lumine_tests "[loader]" # run one tag
./build/tests/lumine_tests --list-tests
```

The test runner is **headless**: it constructs no `QGuiApplication`, so it
runs without a display. CTest also sets `QT_QPA_PLATFORM=offscreen` as a
safety net for future tests.

## What is covered (Iteration 1)

`tests/test_image_loader.cpp`:

- decoding a valid image and verifying its dimensions
- a missing file path → `LoadError::FileNotFound`
- a corrupt file (valid signature, garbage body) → decode failure
- a file with an unsupported extension → load failure
- the supported-suffix list is non-empty and contains `png`
- case-insensitive suffix recognition

`tests/test_platform_info.cpp`:

- session type and desktop environment mirror the process environment
- the platform name is empty without a `QGuiApplication`

Test fixtures are generated at runtime into a `QTemporaryDir` — no binary
assets are committed to the repository, and every run starts clean.

## Planned coverage

As the matching features land:

- async load cancellation and rapid navigation (Phase 2)
- cache eviction and LRU ordering (Phase 3)
- zoom boundary clamping (Phase 2)
- very large image handling and progressive loading (Phase 5)

## Adding a test

1. Create `tests/test_<area>.cpp`.
2. Add the file to `lumine_tests` in `tests/CMakeLists.txt`.
3. Use `TEST_CASE("description", "[tag]")` with a descriptive tag.
4. Keep tests headless — exercise logic, not widgets, wherever possible.
