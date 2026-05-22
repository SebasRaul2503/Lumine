# Packaging

## Install from source

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
sudo cmake --install build
```

This installs:

| File                                                       | Purpose            |
| ---------------------------------------------------------- | ------------------ |
| `/usr/bin/lumine`                                          | The executable     |
| `/usr/share/applications/io.github.lumine.Lumine.desktop`  | Desktop entry      |
| `/usr/share/icons/hicolor/scalable/apps/io.github.lumine.Lumine.svg` | App icon |

On Wayland the compositor resolves the icon and app identity from the desktop
entry via the surface `app_id` — see [architecture.md](architecture.md).

## Arch Linux package

A VCS `PKGBUILD` is provided in [`packaging/PKGBUILD`](../packaging/PKGBUILD).

```sh
cp packaging/PKGBUILD /tmp/lumine-build/
cd /tmp/lumine-build
makepkg -si
```

Before publishing to the AUR, set the real repository URL in `url` and
`source`, then generate the metadata:

```sh
makepkg --printsrcinfo > .SRCINFO
```

Runtime dependencies: `qt6-base`, `qt6-wayland`, `hicolor-icon-theme`.
Build dependencies: `cmake`, `ninja`, `git`, `catch2`.

## Tarballs via CPack

The CMake build is CPack-enabled:

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cd build

cpack                                  # binary tarball  (lumine-<version>-*.tar.gz)
cpack --config CPackSourceConfig.cmake  # source tarball
```
