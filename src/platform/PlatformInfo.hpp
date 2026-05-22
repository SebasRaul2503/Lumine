#pragma once

#include <QString>

namespace lumine::platform {

// Reports the windowing environment Lumine is running under. Used to verify
// Wayland-native operation and to drive platform-specific behaviour later.
class PlatformInfo {
public:
    // The active Qt platform plugin, e.g. "wayland", "xcb" or "offscreen".
    // Empty when no QGuiApplication has been constructed yet.
    [[nodiscard]] static QString platformName();

    // True when Lumine is running on a Wayland compositor such as Hyprland.
    [[nodiscard]] static bool isWayland();

    // Value of $XDG_SESSION_TYPE, e.g. "wayland" or "x11" (may be empty).
    [[nodiscard]] static QString sessionType();

    // Value of $XDG_CURRENT_DESKTOP, e.g. "Hyprland" (may be empty).
    [[nodiscard]] static QString desktopEnvironment();
};

} // namespace lumine::platform
