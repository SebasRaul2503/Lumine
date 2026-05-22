# Building Lumine

## Dependencies

| Dependency      | Purpose                        | Required           |
| --------------- | ------------------------------ | ------------------ |
| C++20 compiler  | GCC ≥ 12 or Clang ≥ 15         | Yes                |
| CMake ≥ 3.21    | Build system                   | Yes                |
| Qt 6 (≥ 6.5)    | Core, Gui, Widgets, OpenGLWidgets | Yes             |
| qt6-wayland     | Wayland platform plugin        | Yes (to run on Wayland) |
| Catch2 v3       | Test framework                 | Tests only \*      |
| clang-format / clang-tidy | Formatting & linting | Development only   |
| Ninja or Make   | Build driver                   | Yes (either)       |

\* If Catch2 is not installed, CMake fetches it from source automatically
(requires network access at configure time). To skip tests entirely,
configure with `-DLUMINE_BUILD_TESTS=OFF`.

### Arch Linux

```sh
sudo pacman -S --needed base-devel cmake qt6-base qt6-wayland catch2 clang
```

`base-devel` provides GCC, Make and Git. `ninja` is optional but recommended:

```sh
sudo pacman -S --needed ninja
```

## Configure and build

From the repository root:

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Add `-G Ninja` to the configure step to use the Ninja generator.

The executable is produced at `build/src/lumine`.

## Build types

| Type             | Use                                              |
| ---------------- | ------------------------------------------------ |
| `Release`        | Default. Optimised, for everyday use.            |
| `Debug`          | Assertions and debug symbols, for development.   |
| `RelWithDebInfo` | Optimised with symbols, for profiling.           |

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```

## Build options

| Option                     | Default | Effect                              |
| -------------------------- | ------- | ----------------------------------- |
| `LUMINE_BUILD_TESTS`       | `ON`    | Build the Catch2 test suite.        |
| `LUMINE_WARNINGS_AS_ERRORS`| `OFF`   | Treat compiler warnings as errors.  |

Example:

```sh
cmake -B build -DLUMINE_WARNINGS_AS_ERRORS=ON
```

## Running

```sh
# Open an image directly
./build/src/lumine ~/Pictures/photo.jpg

# Launch with no image and use Ctrl+O
./build/src/lumine

# Verbose logging
QT_LOGGING_RULES="lumine.*=true" ./build/src/lumine ~/Pictures/photo.jpg
```

## Installing

```sh
sudo cmake --install build
```

This installs the `lumine` binary, the `.desktop` entry and the application
icon under the standard prefix (`/usr/local` by default). Pass
`-DCMAKE_INSTALL_PREFIX=/usr` at configure time to install system-wide.

## Troubleshooting

- **`Could not find a package configuration file provided by "Qt6"`** —
  install `qt6-base`.
- **Window falls back to XWayland** — install `qt6-wayland`. Force the plugin
  with `QT_QPA_PLATFORM=wayland ./build/src/lumine`.
- **Catch2 fetch fails offline** — install the `catch2` package, or configure
  with `-DLUMINE_BUILD_TESTS=OFF`.
